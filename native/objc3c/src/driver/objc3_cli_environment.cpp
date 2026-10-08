#include "driver/objc3_cli_environment.h"

#include <cstdlib>
#include <system_error>

namespace {

std::filesystem::path Objc3LlvmToolExecutableName(const char *tool_name) {
#if defined(_WIN32)
  const std::filesystem::path tool_path(tool_name);
  if (tool_path.extension() != ".exe") {
    return std::filesystem::path(std::string(tool_name) + ".exe");
  }
#endif
  return std::filesystem::path(tool_name);
}

std::filesystem::path Objc3LlvmRootToolPath(
    const std::string &root,
    const std::filesystem::path &tool_name) {
  return std::filesystem::path(root) / "bin" / tool_name;
}

}  // namespace

std::string ReadObjc3DriverEnvironmentVariable(const char *name) {
#if defined(_WIN32)
  char *value = nullptr;
  std::size_t value_length = 0;
  if (_dupenv_s(&value, &value_length, name) != 0 || value == nullptr ||
      value_length == 0) {
    if (value != nullptr) {
      std::free(value);
    }
    return "";
  }
  std::string result(value);
  std::free(value);
  return result;
#else
  const char *value = std::getenv(name);
  return value == nullptr ? "" : std::string(value);
#endif
}

std::filesystem::path DefaultObjc3DriverLlvmToolPath(const char *tool_name) {
  const std::filesystem::path executable_name =
      Objc3LlvmToolExecutableName(tool_name);
  const std::string objc3c_llvm_root =
      ReadObjc3DriverEnvironmentVariable("OBJC3C_LLVM_ROOT");
  if (!objc3c_llvm_root.empty()) {
    return Objc3LlvmRootToolPath(objc3c_llvm_root, executable_name);
  }

  const std::string llvm_root = ReadObjc3DriverEnvironmentVariable("LLVM_ROOT");
  if (!llvm_root.empty()) {
    return Objc3LlvmRootToolPath(llvm_root, executable_name);
  }

#if defined(_WIN32)
  const std::string user_profile =
      ReadObjc3DriverEnvironmentVariable("USERPROFILE");
  if (!user_profile.empty()) {
    std::string version =
        ReadObjc3DriverEnvironmentVariable("OBJC3C_CI_LLVM_VERSION");
    if (version.empty()) {
      version = "22.1.6";
    }
    const std::filesystem::path user_tool =
        std::filesystem::path(user_profile) / "Tools" / "LLVM" /
        ("llvm-" + version + "-msvc") / "bin" / executable_name;
    std::error_code error;
    if (std::filesystem::is_regular_file(user_tool, error)) {
      return user_tool;
    }
  }
#endif

  return executable_name;
}

std::filesystem::path DefaultObjc3DriverClangPath() {
  return DefaultObjc3DriverLlvmToolPath("clang");
}

std::filesystem::path DefaultObjc3DriverLlcPath() {
  const std::string configured_llc =
      ReadObjc3DriverEnvironmentVariable("OBJC3C_NATIVE_EXECUTION_LLC_PATH");
  if (!configured_llc.empty()) {
    return configured_llc;
  }
  return DefaultObjc3DriverLlvmToolPath("llc");
}

void ApplyObjc3CliEnvironmentDefaults(Objc3CliOptions &options) {
  if (!options.clang_path_explicit) {
    options.clang_path = DefaultObjc3DriverClangPath();
  }
  if (!options.llc_path_explicit) {
    options.llc_path = DefaultObjc3DriverLlcPath();
  }
  const std::string metaprogramming_cache_root =
      ReadObjc3DriverEnvironmentVariable("OBJC3C_METAPROGRAMMING_CACHE_ROOT");
  if (!metaprogramming_cache_root.empty()) {
    options.metaprogramming_cache_root = metaprogramming_cache_root;
  }
}
