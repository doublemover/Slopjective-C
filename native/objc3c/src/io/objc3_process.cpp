#include "io/objc3_process.h"

#include <process.h>

#include <string>
#include <vector>

int RunProcess(const std::string &executable, const std::vector<std::string> &args) {
  std::vector<const char *> argv;
  argv.reserve(args.size() + 2);
  argv.push_back(executable.c_str());
  for (const auto &arg : args) {
    argv.push_back(arg.c_str());
  }
  argv.push_back(nullptr);

  const int status = _spawnvp(_P_WAIT, executable.c_str(), argv.data());
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
