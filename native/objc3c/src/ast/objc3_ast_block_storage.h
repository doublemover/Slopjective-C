#pragma once

#include <cstddef>
#include <string>

std::string BuildBlockStorageEscapeProfile(std::size_t mutable_capture_count,
                                           std::size_t byref_slot_count,
                                           bool escape_to_heap,
                                           std::size_t body_statement_count);
std::string BuildBlockStorageByrefLayoutSymbol(unsigned line,
                                               unsigned column,
                                               std::size_t mutable_capture_count,
                                               std::size_t byref_slot_count,
                                               bool escape_to_heap);
std::string BuildBlockCopyDisposeProfile(std::size_t mutable_capture_count,
                                         std::size_t byref_slot_count,
                                         bool escape_to_heap,
                                         std::size_t body_statement_count);
std::string BuildBlockCopyHelperSymbol(unsigned line,
                                       unsigned column,
                                       std::size_t mutable_capture_count,
                                       std::size_t byref_slot_count,
                                       bool escape_to_heap);
std::string BuildBlockDisposeHelperSymbol(unsigned line,
                                          unsigned column,
                                          std::size_t mutable_capture_count,
                                          std::size_t byref_slot_count,
                                          bool escape_to_heap);
