#include "runtime/public/objc3_runtime_result_contract.h"

#include "runtime/public/objc3_runtime_result_builder.h"

namespace objc3c::runtime {

objc3_runtime_dispatch_i32_result MakeRuntimeDispatchI32Result(
    objc3_runtime_dispatch_status_code status_code, int value) {
  return BuildRuntimeDispatchI32Result(status_code, value);
}

}  // namespace objc3c::runtime
