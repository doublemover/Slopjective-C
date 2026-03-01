#include "io/objc3_process.h"

#include <process.h>

#include <string>
#include <vector>

int RunProcess(const std::string &executable, const std::vector<std::string> &args) {
  std::vector<std::string> owned_argv;
  owned_argv.reserve(args.size() + 1);
  owned_argv.push_back(executable);
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

  return RunProcess(clang_exe, {"-x", "objective-c", "-std=gnu11", "-c", input.string(), "-o",
                                object_out.string(), "-fno-color-diagnostics"});
}

int RunIRCompile(const std::filesystem::path &clang_path,
                 const std::filesystem::path &ir_path,
                 const std::filesystem::path &object_out) {
  const std::string clang_exe = clang_path.string();
  return RunProcess(clang_exe, {"-x", "ir", "-c", ir_path.string(), "-o", object_out.string(),
                                "-fno-color-diagnostics"});
}

int RunIRCompileLLVMDirect(const std::filesystem::path &llc_path,
                           const std::filesystem::path &ir_path,
                           const std::filesystem::path &object_out,
                           std::string &error) {
#if defined(OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION)
  const int llc_status =
      RunProcess(llc_path.string(), {"-filetype=obj", "-o", object_out.string(), ir_path.string()});
  if (llc_status == 0) {
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
