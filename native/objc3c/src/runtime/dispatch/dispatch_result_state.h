#pragma once

#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/public/objc3_runtime_result.h"

namespace objc3c::runtime {

struct RuntimeState;

void StoreDispatchResultContractUnlocked(
    RuntimeState &state, objc3_runtime_dispatch_status_code status_code,
    RuntimeMethodReturnKind return_kind);
void RecordTypedDispatchSuccess(RuntimeState &state,
                                RuntimeMethodReturnKind return_kind);
void RecordPostResolutionStrictDispatchFailure(
    RuntimeState &state, objc3_runtime_dispatch_status_code status_code,
    RuntimeMethodReturnKind return_kind, const char *dispatch_path);

}  // namespace objc3c::runtime
