#pragma once

#include <filesystem>
#include <string>
#include <vector>

struct Objc3LLVMCapabilitySummary {
  std::string mode;
  std::string clang_path;
  bool clang_found = false;
  std::string llc_path;
  bool llc_found = false;
  bool llc_supports_filetype_obj = false;
  bool llc_supports_target_object_emission = false;
  bool ok = false;
  bool toolchain_identity_claimable = false;
  std::string native_object_emission_status;
  bool parity_ready = false;
  std::vector<std::string> blockers;
  std::vector<std::string> toolchain_identity_diagnostics;
};

bool TryLoadObjc3LLVMCapabilitySummary(
    const std::filesystem::path &summary_path,
    Objc3LLVMCapabilitySummary &summary,
    std::string &error);
std::string DescribeObjc3LLVMCapabilityBlockers(
    const std::vector<std::string> &blockers);
