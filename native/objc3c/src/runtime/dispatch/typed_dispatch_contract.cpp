#include "runtime/dispatch/typed_dispatch_contract.h"

#include "runtime/dispatch/dispatch_status.h"

namespace objc3c::runtime {

RuntimeTypedDispatchResultContract RuntimeTypedDispatchContractForStatus(
    objc3_runtime_dispatch_status_code status_code) {
  return RuntimeDispatchStatusCarriesValueResult(status_code)
             ? RuntimeTypedDispatchResultContract::ValueResult
             : RuntimeTypedDispatchResultContract::StrictErrorResult;
}

const char *RuntimeTypedDispatchContractName(
    RuntimeTypedDispatchResultContract contract) {
  switch (contract) {
    case RuntimeTypedDispatchResultContract::ValueResult:
      return "typed-dispatch-value-result";
    case RuntimeTypedDispatchResultContract::StrictErrorResult:
      return "typed-dispatch-strict-error-result";
  }
  return "typed-dispatch-strict-error-result";
}

}  // namespace objc3c::runtime
