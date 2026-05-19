#pragma once

#include <cstdint>
#include <string_view>

namespace objc3c::adr {

enum class Objc3AdrStatus : uint8_t {
  Accepted,
};

struct Objc3AdrRecord {
  uint16_t number = 0;
  std::string_view slug;
  std::string_view title;
  Objc3AdrStatus status = Objc3AdrStatus::Accepted;
  std::string_view date;
  std::string_view deciders;
  std::string_view related_surfaces;
};

std::string_view Objc3AdrStatusName(Objc3AdrStatus status);

std::string_view Objc3AdrFileName(const Objc3AdrRecord &record);

bool Objc3AdrRecordIsWellFormed(const Objc3AdrRecord &record);

}  // namespace objc3c::adr
