#pragma once

#include <cstddef>
#include <filesystem>
#include <string>

enum class Objc3IrObjectBackend {
  kClang,
  kLLVMDirect,
};

struct Objc3CliOptions {
  std::filesystem::path input;
  std::filesystem::path out_dir = std::filesystem::path("artifacts") / "compilation" / "objc3c-native";
  std::string emit_prefix = "module";
  std::filesystem::path clang_path = std::filesystem::path("clang");
  std::filesystem::path llc_path = std::filesystem::path("llc");
  Objc3IrObjectBackend ir_object_backend = Objc3IrObjectBackend::kLLVMDirect;
  std::size_t max_message_send_args = 4;
  std::string runtime_dispatch_symbol = "objc3_msgsend_i32";
};

std::string Objc3CliUsage();
bool ParseObjc3CliOptions(int argc, char **argv, Objc3CliOptions &options, std::string &error);
