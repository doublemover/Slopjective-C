#pragma once

namespace objc3c::runtime {

struct RuntimeContinuationState;

int HandoffRuntimeAsyncContinuationToExecutor(
    RuntimeContinuationState &state,
    int continuation_handle,
    int executor_tag);
int ResumeRuntimeAsyncContinuation(RuntimeContinuationState &state,
                                   int continuation_handle,
                                   int result_value);

}  // namespace objc3c::runtime
