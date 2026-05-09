#include "lower/contracts/runtime_dispatch_boundary_contracts.h"

#include <string>

bool IsValidRuntimeDispatchSymbol(const std::string &symbol) {
  return symbol == kObjc3RuntimeDispatchSymbol;
}

bool TryNormalizeObjc3LoweringContract(const Objc3LoweringContract &input,
                                       Objc3LoweringContract &normalized,
                                       std::string &error) {
  if (input.max_message_send_args > kObjc3RuntimeDispatchMaxArgs) {
    error = "invalid lowering contract max_message_send_args: " +
            std::to_string(input.max_message_send_args) + " (expected <= " +
            std::to_string(kObjc3RuntimeDispatchMaxArgs) + ")";
    return false;
  }
  if (!IsValidRuntimeDispatchSymbol(input.runtime_dispatch_symbol)) {
    error =
        "invalid lowering contract runtime_dispatch_symbol (expected objc3_runtime_dispatch_i32): " +
        input.runtime_dispatch_symbol;
    return false;
  }
  if (!Objc3RuntimeDispatchLoweringOwnerIsReady()) {
    error = "runtime dispatch lowering owner contract is not hard-cutover ready";
    return false;
  }
  normalized.max_message_send_args = input.max_message_send_args;
  normalized.runtime_dispatch_symbol = input.runtime_dispatch_symbol;
  return true;
}

bool TryBuildObjc3LoweringIRBoundary(const Objc3LoweringContract &input,
                                     Objc3LoweringIRBoundary &boundary,
                                     std::string &error) {
  Objc3LoweringContract normalized;
  if (!TryNormalizeObjc3LoweringContract(input, normalized, error)) {
    return false;
  }
  boundary.runtime_dispatch_arg_slots = normalized.max_message_send_args;
  boundary.runtime_dispatch_symbol = normalized.runtime_dispatch_symbol;
  boundary.selector_global_ordering = kObjc3SelectorGlobalOrdering;
  return true;
}

std::string Objc3LoweringIRBoundaryReplayKey(
    const Objc3LoweringIRBoundary &boundary) {
  return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +
         ";runtime_dispatch_arg_slots=" +
         std::to_string(boundary.runtime_dispatch_arg_slots) +
         ";selector_global_ordering=" + boundary.selector_global_ordering +
         ";" + Objc3RuntimeDispatchLoweringOwnerReplayKey();
}

bool UsesCanonicalObjc3RuntimeDispatchEntrypoint(
    const std::string &dispatch_surface_family) {
  return dispatch_surface_family == kObjc3DispatchSurfaceInstanceFamily ||
         dispatch_surface_family == kObjc3DispatchSurfaceClassFamily ||
         dispatch_surface_family == kObjc3DispatchSurfaceSuperFamily ||
         dispatch_surface_family == kObjc3DispatchSurfaceDynamicFamily;
}

bool RequiresFailClosedObjc3RuntimeDispatchError(
    const std::string &dispatch_surface_family) {
  return dispatch_surface_family == kObjc3DispatchSurfaceDirectFamily;
}

const char *Objc3DispatchSurfaceRuntimeEntrypointSymbol(
    const std::string &dispatch_surface_family) {
  return UsesCanonicalObjc3RuntimeDispatchEntrypoint(dispatch_surface_family)
             ? kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol
             : "";
}
