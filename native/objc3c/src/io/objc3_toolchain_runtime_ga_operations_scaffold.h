#pragma once

#include <filesystem>
#include <sstream>
#include <string>

struct Objc3ToolchainRuntimeGaOperationsScaffold {
  bool clang_backend_selected = false;
  bool llvm_direct_backend_selected = false;
  bool clang_path_configured = false;
  bool llc_path_configured = false;
  bool llvm_direct_backend_enabled = false;
  bool ir_artifact_ready = false;
  bool object_artifact_ready = false;
  bool compile_route_ready = false;
  bool modular_split_ready = false;
  std::string backend_route_key;
  std::string scaffold_key;
  std::string failure_reason;
};

inline std::string BuildObjc3ToolchainRuntimeGaOperationsScaffoldKey(
    const Objc3ToolchainRuntimeGaOperationsScaffold &scaffold) {
  std::ostringstream key;
  key << "toolchain-runtime-ga-operations-scaffold:v1:"
      << "backend=" << scaffold.backend_route_key
      << ";clang_backend_selected=" << (scaffold.clang_backend_selected ? "true" : "false")
      << ";llvm_direct_backend_selected=" << (scaffold.llvm_direct_backend_selected ? "true" : "false")
      << ";clang_path_configured=" << (scaffold.clang_path_configured ? "true" : "false")
      << ";llc_path_configured=" << (scaffold.llc_path_configured ? "true" : "false")
      << ";llvm_direct_backend_enabled=" << (scaffold.llvm_direct_backend_enabled ? "true" : "false")
      << ";ir_artifact_ready=" << (scaffold.ir_artifact_ready ? "true" : "false")
      << ";object_artifact_ready=" << (scaffold.object_artifact_ready ? "true" : "false")
      << ";compile_route_ready=" << (scaffold.compile_route_ready ? "true" : "false")
      << ";modular_split_ready=" << (scaffold.modular_split_ready ? "true" : "false");
  return key.str();
}

inline Objc3ToolchainRuntimeGaOperationsScaffold BuildObjc3ToolchainRuntimeGaOperationsScaffold(
    bool clang_backend_selected,
    bool llvm_direct_backend_selected,
    const std::filesystem::path &clang_path,
    const std::filesystem::path &llc_path,
    bool llvm_direct_backend_enabled,
    const std::filesystem::path &ir_path,
    const std::filesystem::path &object_out) {
  Objc3ToolchainRuntimeGaOperationsScaffold scaffold;
  scaffold.clang_backend_selected = clang_backend_selected;
  scaffold.llvm_direct_backend_selected = llvm_direct_backend_selected;
  scaffold.clang_path_configured = clang_path.has_filename();
  scaffold.llc_path_configured = llc_path.has_filename();
  scaffold.llvm_direct_backend_enabled = llvm_direct_backend_enabled;
  scaffold.ir_artifact_ready = ir_path.has_filename() && ir_path.extension() == ".ll";
  scaffold.object_artifact_ready = object_out.has_filename() && object_out.extension() == ".obj";

  if (scaffold.clang_backend_selected) {
    scaffold.backend_route_key = "clang";
  } else if (scaffold.llvm_direct_backend_selected) {
    scaffold.backend_route_key = "llvm-direct";
  } else {
    scaffold.backend_route_key = "invalid";
  }

  const bool backend_selection_valid =
      scaffold.clang_backend_selected != scaffold.llvm_direct_backend_selected;
  const bool backend_ready = scaffold.clang_backend_selected
                                 ? scaffold.clang_path_configured
                                 : (scaffold.llvm_direct_backend_selected &&
                                    scaffold.llvm_direct_backend_enabled &&
                                    scaffold.llc_path_configured);
  scaffold.compile_route_ready =
      backend_selection_valid &&
      backend_ready &&
      scaffold.ir_artifact_ready &&
      scaffold.object_artifact_ready;
  scaffold.modular_split_ready = scaffold.compile_route_ready &&
                                 (scaffold.clang_backend_selected != scaffold.llvm_direct_backend_selected);
  scaffold.scaffold_key = BuildObjc3ToolchainRuntimeGaOperationsScaffoldKey(scaffold);

  if (scaffold.modular_split_ready) {
    return scaffold;
  }

  if (!backend_selection_valid) {
    scaffold.failure_reason = "backend route selection must resolve to exactly one backend";
  } else if (scaffold.clang_backend_selected && !scaffold.clang_path_configured) {
    scaffold.failure_reason = "clang backend selected without configured clang path";
  } else if (scaffold.llvm_direct_backend_selected && !scaffold.llvm_direct_backend_enabled) {
    scaffold.failure_reason = "llvm-direct backend unavailable in this build";
  } else if (scaffold.llvm_direct_backend_selected && !scaffold.llc_path_configured) {
    scaffold.failure_reason = "llvm-direct backend selected without configured llc path";
  } else if (!scaffold.ir_artifact_ready) {
    scaffold.failure_reason = "llvm ir artifact path is not ready";
  } else if (!scaffold.object_artifact_ready) {
    scaffold.failure_reason = "object artifact path is not ready";
  } else if (!scaffold.compile_route_ready) {
    scaffold.failure_reason = "toolchain/runtime compile route is not ready";
  } else {
    scaffold.failure_reason = "toolchain/runtime modular split scaffold not ready";
  }

  return scaffold;
}

inline bool IsObjc3ToolchainRuntimeGaOperationsScaffoldReady(
    const Objc3ToolchainRuntimeGaOperationsScaffold &scaffold,
    std::string &reason) {
  if (scaffold.modular_split_ready) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty() ? "toolchain/runtime ga operations scaffold not ready"
                                           : scaffold.failure_reason;
  return false;
}
