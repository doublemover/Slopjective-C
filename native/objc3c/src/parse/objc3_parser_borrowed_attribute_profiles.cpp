#include "parse/objc3_parser_attributes.h"

#include <sstream>

namespace objc3c::parse {

bool IsObjc3BorrowedQualifierSpelling(const std::string &text) {
  return text == "borrowed";
}

std::string BuildObjc3ReturnsBorrowedProfile(
    bool declared,
    std::size_t owner_index,
    bool return_borrowed_pointer_qualified) {
  std::ostringstream out;
  out << "returns-borrowed:declared=" << (declared ? "true" : "false")
      << ";owner_index=" << owner_index
      << ";return_borrowed="
      << (return_borrowed_pointer_qualified ? "true" : "false");
  return out.str();
}

}  // namespace objc3c::parse
