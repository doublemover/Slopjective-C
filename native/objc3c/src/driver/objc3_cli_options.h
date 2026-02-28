#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>

enum class Objc3IrObjectBackend {
  kClang,
  kLLVMDirect,
};

enum class Objc3CompatMode {
  kCanonical,
  kLegacy,
};

struct Objc3CliOptions {
  std::filesystem::path input;
  std::filesystem::path out_dir = std::filesystem::path("tmp") / "artifacts" / "compilation" / "objc3c-native";
  std::string emit_prefix = "module";
  std::filesystem::path clang_path = std::filesystem::path("clang");
  std::filesystem::path llc_path = std::filesystem::path("llc");
  std::filesystem::path llvm_capabilities_summary;
  bool route_backend_from_capabilities = false;
  bool clang_path_explicit = false;
  bool llc_path_explicit = false;
  Objc3IrObjectBackend ir_object_backend = Objc3IrObjectBackend::kLLVMDirect;
  std::uint32_t language_version = 3;
  Objc3CompatMode compat_mode = Objc3CompatMode::kCanonical;
  bool migration_assist = false;
  std::size_t max_message_send_args = 4;
  std::string runtime_dispatch_symbol = "objc3_msgsend_i32";
};

std::string Objc3CliUsage();
bool ParseObjc3CliOptions(int argc, char **argv, Objc3CliOptions &options, std::string &error);
