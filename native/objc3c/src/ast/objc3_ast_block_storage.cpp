#include "ast/objc3_ast_block_storage.h"

#include <sstream>

std::string BuildBlockStorageEscapeProfile(std::size_t mutable_capture_count,
                                           std::size_t byref_slot_count,
                                           bool escape_to_heap,
                                           std::size_t body_statement_count) {
  std::ostringstream out;
  out << "block-storage:mutable-captures=" << mutable_capture_count
      << ";byref-slots=" << byref_slot_count
      << ";escape=" << (escape_to_heap ? "heap" : "stack")
      << ";body-statements=" << body_statement_count;
  return out.str();
}

std::string BuildBlockStorageByrefLayoutSymbol(unsigned line,
                                               unsigned column,
                                               std::size_t mutable_capture_count,
                                               std::size_t byref_slot_count,
                                               bool escape_to_heap) {
  std::ostringstream out;
  out << "__objc3_block_byref_layout_" << line << "_" << column
      << "_m" << mutable_capture_count
      << "_b" << byref_slot_count
      << "_" << (escape_to_heap ? "heap" : "stack");
  return out.str();
}

std::string BuildBlockCopyDisposeProfile(std::size_t mutable_capture_count,
                                         std::size_t byref_slot_count,
                                         bool escape_to_heap,
                                         std::size_t body_statement_count) {
  std::ostringstream out;
  out << "block-copy-dispose:copy-helper="
      << (mutable_capture_count > 0u ? "enabled" : "elided")
      << ";dispose-helper=" << (byref_slot_count > 0u ? "enabled" : "elided")
      << ";escape=" << (escape_to_heap ? "heap" : "stack")
      << ";body-statements=" << body_statement_count;
  return out.str();
}

std::string BuildBlockCopyHelperSymbol(unsigned line,
                                       unsigned column,
                                       std::size_t mutable_capture_count,
                                       std::size_t byref_slot_count,
                                       bool escape_to_heap) {
  std::ostringstream out;
  out << "__objc3_block_copy_helper_" << line << "_" << column
      << "_m" << mutable_capture_count
      << "_b" << byref_slot_count
      << "_" << (escape_to_heap ? "heap" : "stack");
  return out.str();
}

std::string BuildBlockDisposeHelperSymbol(unsigned line,
                                          unsigned column,
                                          std::size_t mutable_capture_count,
                                          std::size_t byref_slot_count,
                                          bool escape_to_heap) {
  std::ostringstream out;
  out << "__objc3_block_dispose_helper_" << line << "_" << column
      << "_m" << mutable_capture_count
      << "_b" << byref_slot_count
      << "_" << (escape_to_heap ? "heap" : "stack");
  return out.str();
}
