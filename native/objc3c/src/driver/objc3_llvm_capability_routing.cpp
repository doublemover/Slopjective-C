#include "driver/objc3_llvm_capability_routing.h"

#include <string>

#include "driver/objc3_llvm_capability_summary.h"

bool ApplyObjc3LLVMCapabilityRouting(Objc3CliOptions &options,
                                     std::string &error) {
  if (options.llvm_capabilities_summary.empty()) {
    if (options.route_backend_from_capabilities) {
      error = "capability routing fail-closed: --objc3-route-backend-from-capabilities requires --llvm-capabilities-summary";
      return false;
    }
    return true;
  }

  Objc3LLVMCapabilitySummary summary;
  if (!TryLoadObjc3LLVMCapabilitySummary(
          options.llvm_capabilities_summary, summary, error)) {
    error = "capability routing fail-closed: " + error;
    return false;
  }

  if (!summary.parity_ready) {
    error = "capability routing fail-closed: sema/type-system parity capability unavailable: " +
            DescribeObjc3LLVMCapabilityBlockers(summary.blockers);
    return false;
  }

  if (!options.clang_path_explicit && !summary.clang_path.empty()) {
    options.clang_path = summary.clang_path;
  }
  if (!options.llc_path_explicit && !summary.llc_path.empty()) {
    options.llc_path = summary.llc_path;
  }

  if (options.route_backend_from_capabilities) {
    options.ir_object_backend =
        summary.llc_supports_filetype_obj ? Objc3IrObjectBackend::kLLVMDirect : Objc3IrObjectBackend::kClang;
  }

  if (options.ir_object_backend == Objc3IrObjectBackend::kClang && !summary.clang_found) {
    error = "capability routing fail-closed: clang backend selected but capability summary reports clang unavailable";
    return false;
  }
  if (options.ir_object_backend == Objc3IrObjectBackend::kLLVMDirect &&
      (!summary.llc_found || !summary.llc_supports_filetype_obj)) {
    error =
        "capability routing fail-closed: llvm-direct backend selected but llc --filetype=obj capability is unavailable";
    return false;
  }
  return true;
}
