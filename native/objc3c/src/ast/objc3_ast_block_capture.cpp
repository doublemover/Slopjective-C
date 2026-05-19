#include "ast/objc3_ast_block_capture.h"

#include <sstream>
#include <utility>

#include "ast/objc3_ast_ordering.h"

std::string BuildBlockLiteralCaptureProfile(
    const std::vector<std::string> &capture_names_lexicographic) {
  if (capture_names_lexicographic.empty()) {
    return "block-captures:none";
  }
  std::ostringstream out;
  out << "block-captures:";
  for (std::size_t i = 0; i < capture_names_lexicographic.size(); ++i) {
    out << capture_names_lexicographic[i];
    if (i + 1u != capture_names_lexicographic.size()) {
      out << ",";
    }
  }
  return out.str();
}

std::string BuildBlockCaptureInventoryEntry(const std::string &capture_name) {
  return "name=" + capture_name +
         ";storage=by-value-readonly;byref=false;mutable=false";
}

std::vector<std::string> BuildBlockCaptureInventoryEntriesLexicographic(
    const std::vector<std::string> &capture_names_lexicographic) {
  std::vector<std::string> entries;
  entries.reserve(capture_names_lexicographic.size());
  for (const auto &capture_name : capture_names_lexicographic) {
    entries.push_back(BuildBlockCaptureInventoryEntry(capture_name));
  }
  return Objc3AstSortedUniqueStrings(std::move(entries));
}

std::string BuildBlockCaptureInventoryProfile(
    std::size_t capture_count,
    std::size_t byvalue_readonly_capture_count) {
  std::ostringstream out;
  out << "block-capture-inventory:captures=" << capture_count
      << ";byvalue-readonly=" << byvalue_readonly_capture_count
      << ";byref=0;mutable=0";
  return out.str();
}
