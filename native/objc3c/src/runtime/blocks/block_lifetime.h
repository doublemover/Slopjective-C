#pragma once

#include "runtime/blocks/block_runtime_records.h"

#include <vector>

namespace objc3c::runtime {

struct RuntimeState;

inline void DisposeRuntimeBlockRecord(RuntimeBlockRecord &record) {
  if (record.dispose_helper != nullptr && !record.storage_words.empty()) {
    record.dispose_helper(record.storage_words.data());
  }
}

inline void DisposeRuntimeBlockRecords(
    std::vector<RuntimeBlockRecord> &records) {
  for (RuntimeBlockRecord &record : records) {
    DisposeRuntimeBlockRecord(record);
  }
  records.clear();
}

bool RetainRuntimeBlockHandleUnlocked(RuntimeState &state, int block_handle);
bool ReleaseRuntimeBlockHandleUnlocked(
    RuntimeState &state, int block_handle,
    std::vector<RuntimeBlockRecord> *records_to_dispose = nullptr);

}  // namespace objc3c::runtime
