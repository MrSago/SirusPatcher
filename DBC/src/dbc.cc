#include "dbc.h"

#include <cstring>
#include <fstream>

DBCHandler::DBCHandler() : dbc_(nullptr) {}

DBCHandler::~DBCHandler() { Clear(); }

errno_t DBCHandler::Load(const std::string& filename) {
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

errno_t DBCHandler::Save(const std::string& filename) const {
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
  record_id_map_.clear();

  if (!dbc_) return;

  if (dbc_->records) delete[] dbc_->records;
  if (dbc_->string_block) delete[] dbc_->string_block;

  delete dbc_;
  dbc_ = nullptr;
}

Record DBCHandler::operator[](uint32_t index) const {
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

  if (record_id_map_.find(record_id) != record_id_map_.end()) {
    return record_id_map_.at(record_id);
  }

  for (uint32_t i = 0; i < dbc_->header.record_count; ++i) {
    Record record(dbc_, i);
    if (record.GetUInt32(1) == record_id) {
      record_id_map_.insert({record_id, record});
      return record;
    }
  }

  return Record(dbc_, dbc_->header.record_count - 1);
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
