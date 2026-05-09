#pragma once

#include "runtime/public/objc3_runtime_result.h"

namespace objc3c::runtime {

enum class RuntimeTypedDispatchResultContract {
  ValueResult,
  StrictErrorResult,
};

RuntimeTypedDispatchResultContract RuntimeTypedDispatchContractForStatus(
    objc3_runtime_dispatch_status_code status_code);
const char *RuntimeTypedDispatchContractName(
    RuntimeTypedDispatchResultContract contract);

}  // namespace objc3c::runtime
