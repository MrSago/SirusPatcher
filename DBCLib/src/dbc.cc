
#include "dbc.h"

#include <fstream>

DBCHandler::DBCHandler() : dbc_(nullptr) {}

DBCHandler::~DBCHandler() { Clear(); }

DBCError DBCHandler::Load(const std::string& filename) {
  Clear();

  std::ifstream file(filename, std::ios::binary);
  if (file.fail() || !file.is_open()) {
    return DBCError::kFileNotOpen;
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
  std::ofstream file(filename, std::ios::binary);
  if (file.fail() || !file.is_open()) {
    return DBCError::kFileNotOpen;
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
  if (index < 0 || index >= dbc_->header.record_count) {
    throw std::out_of_range("Record index out of range");
  }
  return Record(dbc_, index);
}
