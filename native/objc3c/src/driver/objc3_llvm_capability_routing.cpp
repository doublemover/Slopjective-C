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
  if (!summary.toolchain_identity_claimable) {
    error =
        "capability routing fail-closed: coherent LLVM toolchain identity "
        "is required before object, package, execution, or platform success: " +
        DescribeObjc3LLVMCapabilityBlockers(
            summary.toolchain_identity_diagnostics);
    return false;
  }

  if (!options.clang_path_explicit && !summary.clang_path.empty()) {
    options.clang_path = summary.clang_path;
  }
  if (!options.llc_path_explicit && !summary.llc_path.empty()) {
    options.llc_path = summary.llc_path;
  }

  if (options.route_backend_from_capabilities) {
    if (!summary.llc_found || !summary.llc_supports_filetype_obj ||
        !summary.llc_supports_target_object_emission ||
        summary.native_object_emission_status !=
            "native_object_emission_supported") {
      error =
          "capability routing fail-closed: native object emission requires "
          "llc --filetype=obj, non-empty target object output, and a coherent "
          "LLVM toolchain identity; clang substitute object emission is not "
          "permitted";
      return false;
    }
    options.ir_object_backend = Objc3IrObjectBackend::kLLVMDirect;
  }

  if (options.ir_object_backend == Objc3IrObjectBackend::kClang && !summary.clang_found) {
    error = "capability routing fail-closed: clang backend selected but capability summary reports clang unavailable";
    return false;
  }
  if (options.ir_object_backend == Objc3IrObjectBackend::kLLVMDirect &&
      (!summary.llc_found || !summary.llc_supports_filetype_obj ||
       !summary.llc_supports_target_object_emission ||
       summary.native_object_emission_status !=
           "native_object_emission_supported")) {
    error =
        "capability routing fail-closed: llvm-direct backend selected but llc --filetype=obj target object emission and coherent LLVM toolchain identity are unavailable";
    return false;
  }
  return true;
}
