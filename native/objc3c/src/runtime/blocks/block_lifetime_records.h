#pragma once

#include "runtime/blocks/block_runtime_records.h"
#include "runtime/state/runtime_state_records.h"

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

inline void DisposeRuntimeBlockRecord(RuntimeBlockRecord &record) {
  if (record.dispose_helper != nullptr && !record.storage_words.empty()) {
    record.dispose_helper(record.storage_words.data());
  }
}

inline bool ReleaseRuntimeBlockRecordUnlocked(RuntimeState &state,
                                              int block_handle) {
  const auto block_it = state.runtime_blocks_by_handle.find(block_handle);
  if (block_it == state.runtime_blocks_by_handle.end()) {
    return false;
  }

  RuntimeBlockRecord &record = block_it->second;
  if (RuntimeBlockRecordRemainsLiveAfterRelease(record)) {
    return true;
  }

  DisposeRuntimeBlockRecord(record);
  state.runtime_blocks_by_handle.erase(block_it);
  return true;
}

}  // namespace objc3c::runtime
