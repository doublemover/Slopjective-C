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
  bool parity_ready = false;
  std::vector<std::string> blockers;
};

bool TryLoadObjc3LLVMCapabilitySummary(
    const std::filesystem::path &summary_path,
    Objc3LLVMCapabilitySummary &summary,
    std::string &error);
std::string DescribeObjc3LLVMCapabilityBlockers(
    const std::vector<std::string> &blockers);
