#pragma once

#include "runtime/blocks/block_lifetime.h"
#include "runtime/state/runtime_state_records.h"

#include <utility>

namespace objc3c::runtime {

inline RuntimeBlockRecord *FindRuntimeBlockRecordUnlocked(
    RuntimeState &state,
    int block_handle) {
  const auto block_it = state.runtime_blocks_by_handle.find(block_handle);
  if (block_it == state.runtime_blocks_by_handle.end()) {
    return nullptr;
  }
  return &block_it->second;
}

inline bool RetainRuntimeBlockRecord(RuntimeBlockRecord *record) {
  if (record == nullptr) {
    return false;
  }
  ++record->retain_count;
  return true;
}

inline bool RuntimeBlockRecordRemainsLiveAfterRelease(
    RuntimeBlockRecord &record) {
  if (record.retain_count <= 1u) {
    return false;
  }
  --record.retain_count;
  return true;
}

inline bool ReleaseRuntimeBlockRecordUnlocked(RuntimeState &state,
                                              int block_handle,
                                              std::vector<RuntimeBlockRecord>
                                                  *records_to_dispose) {
  const auto block_it = state.runtime_blocks_by_handle.find(block_handle);
  if (block_it == state.runtime_blocks_by_handle.end()) {
    return false;
  }

  RuntimeBlockRecord &record = block_it->second;
  if (RuntimeBlockRecordRemainsLiveAfterRelease(record)) {
    return true;
  }

  if (records_to_dispose != nullptr) {
    records_to_dispose->push_back(std::move(record));
  } else {
    DisposeRuntimeBlockRecord(record);
  }
  state.runtime_blocks_by_handle.erase(block_it);
  return true;
}

}  // namespace objc3c::runtime
