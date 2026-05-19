#include "libobjc3c_frontend/frontend_lowering_boundary.h"

#include <string>

#include "libobjc3c_frontend/objc3c_frontend_compile_contract.h"

namespace objc3c::frontend {
namespace {

inline constexpr std::size_t kFrontendRuntimeDispatchMaxArgs = 16;
inline constexpr const char *kFrontendRuntimeDispatchSymbol =
    "objc3_runtime_dispatch_i32";
inline constexpr const char *kFrontendRuntimeDispatchLoweringOwner =
    "native.lower.runtime-dispatch";
inline constexpr const char *kFrontendRuntimeDispatchLoweringOwnerModel =
    "strict-hard-cutover-no-retired-route-no-compatibility-gate";

bool FrontendRuntimeDispatchLoweringOwnerIsReady() {
  const std::string owner = kFrontendRuntimeDispatchLoweringOwner;
  const std::string owner_model = kFrontendRuntimeDispatchLoweringOwnerModel;
  return !owner.empty() && owner.rfind("native.", 0) == 0 &&
         owner_model == kFrontendRuntimeDispatchLoweringOwnerModel;
}

}  // namespace

Objc3FrontendLoweringBoundaryContract BuildFrontendLoweringBoundaryContract(
    const Objc3FrontendOptions &options) {
  return {
      .max_message_send_args = options.lowering.max_message_send_args,
      .runtime_dispatch_symbol = options.lowering.runtime_dispatch_symbol,
      .runtime_dispatch_lowering_owner_ready =
          FrontendRuntimeDispatchLoweringOwnerIsReady(),
  };
}

bool TryNormalizeFrontendLoweringBoundary(Objc3FrontendOptions &options,
                                          std::string &error) {
  const Objc3FrontendLoweringBoundaryContract contract =
      BuildFrontendLoweringBoundaryContract(options);
  if (contract.max_message_send_args > kFrontendRuntimeDispatchMaxArgs) {
    error = "invalid lowering contract max_message_send_args: " +
            std::to_string(contract.max_message_send_args) + " (expected <= " +
            std::to_string(kFrontendRuntimeDispatchMaxArgs) + ")";
    return false;
  }
  if (contract.runtime_dispatch_symbol != kFrontendRuntimeDispatchSymbol) {
    error =
        "invalid lowering contract runtime_dispatch_symbol (expected objc3_runtime_dispatch_i32): " +
        contract.runtime_dispatch_symbol;
    return false;
  }
  if (!contract.runtime_dispatch_lowering_owner_ready) {
    error = "runtime dispatch lowering owner contract is not hard-cutover ready";
    return false;
  }
  return true;
}

}  // namespace objc3c::frontend
