#include "DBC/dbchandler.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <string>

DBCHandler::DBCHandler() : dbc_(nullptr) {}

DBCHandler::~DBCHandler() { Clear(); }

DBCError DBCHandler::Load(const std::string& filename) {
  Clear();

  std::ifstream file(filename, std::ios::binary);
  if (file.fail() || !file.is_open()) {
    return DBCError::kFileOpenError;
  }

  dbc_ = new DBCFile();
  file.read(reinterpret_cast<char*>(&dbc_->header), sizeof(DBCHeader));

  if (dbc_->header.magic != kDBCMagic) {
    delete dbc_;
    return DBCError::kInvalidMagic;
  }

  dbc_->records =
      new char[dbc_->header.record_size * dbc_->header.record_count];
  file.read(dbc_->records,
            dbc_->header.record_size * dbc_->header.record_count);

  dbc_->string_block = new char[dbc_->header.string_block_size];
  file.read(dbc_->string_block, dbc_->header.string_block_size);

  return DBCError::kSuccess;
}

DBCError DBCHandler::Save(const std::string& filename) {
  if (!dbc_) {
    return DBCError::kFileNotOpened;
  }

  std::ofstream file(filename, std::ios::binary);
  if (file.fail() || !file.is_open()) {
    return DBCError::kFileOpenError;
  }

  file.write(reinterpret_cast<char*>(&dbc_->header), sizeof(DBCHeader));
  file.write(dbc_->records,
             dbc_->header.record_size * dbc_->header.record_count);
  file.write(dbc_->string_block, dbc_->header.string_block_size);

  return DBCError::kSuccess;
}

void DBCHandler::Clear() {
  if (!dbc_) return;

  if (dbc_->records) delete[] dbc_->records;
  if (dbc_->string_block) delete[] dbc_->string_block;

  delete dbc_;
  dbc_ = nullptr;
}

Record DBCHandler::operator[](uint32_t index) {
  if (!dbc_) {
    throw std::runtime_error("DBC file not opened");
  }
  if (index < 0 || index >= dbc_->header.record_count) {
    throw std::out_of_range("Record index out of range");
  }

  return Record(dbc_, index);
}

Record DBCHandler::GetRecordById(uint32_t record_id) {
  if (!dbc_) {
    throw std::runtime_error("DBC file not opened");
  }
  if (record_id <= 0 ||
      record_id > Record(dbc_, dbc_->header.record_count - 1).GetUInt32(1)) {
    throw std::out_of_range("Record id out of range");
  }

  return BinSearchRecord(record_id);
}

Record DBCHandler::AllocateNewRecord() {
  if (!dbc_) {
    throw std::runtime_error("DBC file not opened");
  }

  uint32_t index = dbc_->header.record_count++;
  dbc_->records = reinterpret_cast<char*>(realloc(
      dbc_->records, dbc_->header.record_size * dbc_->header.record_count));
  memset(dbc_->records + dbc_->header.record_size * index, 0,
         dbc_->header.record_size);

  Record record(dbc_, index);
  uint32_t id = Record(dbc_, index - 1).GetUInt32(1) + 1;
  record.SetUInt32(1, id);

  return record;
}

Record DBCHandler::BinSearchRecord(uint32_t record_id) {
  uint32_t left = 0;
  uint32_t right = dbc_->header.record_count - 1;

  while (left <= right) {
    uint32_t mid = (left + right) / 2;
    Record record(dbc_, mid);
    uint32_t id = record.GetUInt32(1);
    if (id == record_id) {
      return record;
    } else if (id < record_id) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }

  throw std::out_of_range("Record id not found");
}
