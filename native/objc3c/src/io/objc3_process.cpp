#include "io/objc3_process.h"

#include <process.h>

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace {

bool IsRecognizedCoffMachine(std::uint16_t machine) {
  switch (machine) {
    case 0x014c:  // IMAGE_FILE_MACHINE_I386
    case 0x8664:  // IMAGE_FILE_MACHINE_AMD64
    case 0x01c0:  // IMAGE_FILE_MACHINE_ARM
    case 0xaa64:  // IMAGE_FILE_MACHINE_ARM64
      return true;
    default:
      return false;
  }
}

void NormalizeCoffTimestamp(const std::filesystem::path &object_out) {
  std::error_code file_size_error;
  const std::uintmax_t size = std::filesystem::file_size(object_out, file_size_error);
  if (file_size_error || size < 8) {
    return;
  }

  std::fstream file(object_out, std::ios::in | std::ios::out | std::ios::binary);
  if (!file.is_open()) {
    return;
  }

  std::array<unsigned char, 8> header{};
  file.read(reinterpret_cast<char *>(header.data()), static_cast<std::streamsize>(header.size()));
  if (!file.good() && !file.eof()) {
    return;
  }
  if (file.gcount() != static_cast<std::streamsize>(header.size())) {
    return;
  }

  const std::uint16_t machine = static_cast<std::uint16_t>(header[0]) |
                                static_cast<std::uint16_t>(static_cast<std::uint16_t>(header[1]) << 8u);
  if (!IsRecognizedCoffMachine(machine)) {
    return;
  }

  const char zero_timestamp[4] = {0, 0, 0, 0};
  file.seekp(4, std::ios::beg);
  if (!file.good()) {
    return;
  }
  file.write(zero_timestamp, 4);
}

}  // namespace

int RunProcess(const std::string &executable, const std::vector<std::string> &args) {
  std::vector<std::string> owned_argv;
  owned_argv.reserve(args.size() + 1);
  std::string argv0 = executable;
  const std::filesystem::path executable_path(executable);
  if (executable_path.has_filename()) {
    const std::string filename = executable_path.filename().string();
    if (!filename.empty()) {
      argv0 = filename;
    }
  }
  owned_argv.push_back(argv0);
  for (const auto &arg : args) {
    owned_argv.push_back(arg);
  }

  std::vector<const char *> argv;
  argv.reserve(owned_argv.size() + 1);
  for (const auto &arg : owned_argv) {
    argv.push_back(arg.c_str());
  }
  argv.push_back(nullptr);

  const bool has_explicit_path =
      executable.find('\\') != std::string::npos || executable.find('/') != std::string::npos ||
      executable.find(':') != std::string::npos;
  const int status = has_explicit_path ? _spawnv(_P_WAIT, executable.c_str(), argv.data())
                                       : _spawnvp(_P_WAIT, executable.c_str(), argv.data());
  if (status == -1) {
    return 127;
  }
  return status;
}

int RunObjectiveCCompile(const std::filesystem::path &clang_path,
                         const std::filesystem::path &input,
                         const std::filesystem::path &object_out) {
  const std::string clang_exe = clang_path.string();
  const int syntax_status =
      RunProcess(clang_exe, {"-x", "objective-c", "-std=gnu11", "-fsyntax-only", input.string()});
  if (syntax_status != 0) {
    return syntax_status;
  }

  const int compile_status = RunProcess(clang_exe, {"-x", "objective-c", "-std=gnu11", "-c", input.string(), "-o",
                                                    object_out.string(), "-fno-color-diagnostics"});
  if (compile_status == 0) {
    NormalizeCoffTimestamp(object_out);
  }
  return compile_status;
}

int RunIRCompile(const std::filesystem::path &clang_path,
                 const std::filesystem::path &ir_path,
                 const std::filesystem::path &object_out) {
  const std::string clang_exe = clang_path.string();
  const int compile_status = RunProcess(clang_exe, {"-x", "ir", "-c", ir_path.string(), "-o", object_out.string(),
                                                    "-fno-color-diagnostics"});
  if (compile_status == 0) {
    NormalizeCoffTimestamp(object_out);
  }
  return compile_status;
}

int RunIRCompileLLVMDirect(const std::filesystem::path &llc_path,
                           const std::filesystem::path &ir_path,
                           const std::filesystem::path &object_out,
                           std::string &error) {
#if defined(OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION)
  const int llc_status =
      RunProcess(llc_path.string(), {"-filetype=obj", "-o", object_out.string(), ir_path.string()});
  if (llc_status == 0) {
    NormalizeCoffTimestamp(object_out);
    return 0;
  }
  if (llc_status == 127) {
    error = "llvm-direct object emission failed: llc executable not found: " + llc_path.string();
    return 125;
  }
  error = "llvm-direct object emission failed: llc exited with status " + std::to_string(llc_status) + " for " +
          ir_path.string();
  return llc_status;
#else
  (void)llc_path;
  (void)ir_path;
  (void)object_out;
  error = "llvm-direct object emission backend unavailable in this build (enable OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION).";
  return 125;
#endif
}
