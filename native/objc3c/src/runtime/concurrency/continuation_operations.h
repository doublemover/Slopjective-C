#pragma once

namespace objc3c::runtime {

struct RuntimeContinuationState;

int AllocateRuntimeAsyncContinuation(RuntimeContinuationState &state,
                                     int resume_entry_tag,
                                     int executor_tag);

}  // namespace objc3c::runtime
