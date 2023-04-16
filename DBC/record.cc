#include "dbc/record.h"

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>

#include "dbc/dbchandler.h"

float Record::GetFloat(uint32_t field) const {
  uint32_t index = field - 1;
  ValidateIndex(index);
  return *reinterpret_cast<float*>(pointer_ + sizeof(uint32_t) * index);
}

void Record::SetFloat(uint32_t field, float val) {
  uint32_t index = field - 1;
  ValidateIndex(index);
  *reinterpret_cast<float*>(pointer_ + sizeof(uint32_t) * index) = val;
}

uint32_t Record::GetUInt32(uint32_t field) const {
  uint32_t index = field - 1;
  ValidateIndex(index);
  return *reinterpret_cast<uint32_t*>(pointer_ + sizeof(uint32_t) * index);
}

void Record::SetUInt32(uint32_t field, uint32_t val) {
  uint32_t index = field - 1;
  ValidateIndex(index);
  *reinterpret_cast<uint32_t*>(pointer_ + sizeof(uint32_t) * index) = val;
}

std::string Record::GetString(uint32_t field) const {
  ValidateIndex(field - 1);

  stringref offset = GetUInt32(field);
  if (offset >= dbc_->header.string_block_size)
    throw std::out_of_range("String offset out of range");

  char* string_block_pointer = dbc_->string_block + offset;
  return std::string(string_block_pointer);
}

void Record::SetStringRef(uint32_t field, const std::string& str) {
  ValidateIndex(field - 1);

  stringref offset = GetUInt32(field);
  if (offset == 0 || offset >= dbc_->header.string_block_size)
    throw std::out_of_range("String offset out of range");

  char* string_block_pointer = dbc_->string_block + offset;
  if (str.length() > strlen(string_block_pointer))
    throw std::out_of_range("New string length bigger than allocated space");

  strcpy(string_block_pointer, str.c_str());
}

void Record::AddStringRef(uint32_t field, const std::string& str) {
  ValidateIndex(field - 1);
  stringref offset = dbc_->header.string_block_size;
  SetUInt32(field, offset);

  dbc_->header.string_block_size += str.length() + 1;
  dbc_->string_block = reinterpret_cast<char*>(
      realloc(dbc_->string_block, dbc_->header.string_block_size));

  char* string_block_pointer = dbc_->string_block + offset;
  strcpy(string_block_pointer, str.c_str());
}

Record::Record(DBCFile* dbc, uint32_t index)
    : dbc_(dbc), pointer_(dbc->records + dbc->header.record_size * index) {}

void Record::ValidateIndex(uint32_t index) const {
  if (index < 0 || index >= dbc_->header.field_count)
    throw std::out_of_range("Field index out of range");
}
