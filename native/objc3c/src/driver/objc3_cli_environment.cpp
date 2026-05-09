#include "driver/objc3_cli_environment.h"

#include <cstdlib>

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

std::filesystem::path DefaultObjc3DriverLlcPath() {
#if defined(_WIN32)
  constexpr const char *llc_name = "llc.exe";
#else
  constexpr const char *llc_name = "llc";
#endif
  const std::string llvm_root =
      ReadObjc3DriverEnvironmentVariable("LLVM_ROOT");
  if (!llvm_root.empty()) {
    return std::filesystem::path(llvm_root) / "bin" / llc_name;
  }
  return std::filesystem::path(llc_name);
}

void ApplyObjc3CliEnvironmentDefaults(Objc3CliOptions &options) {
  options.llc_path = DefaultObjc3DriverLlcPath();
  const std::string metaprogramming_cache_root =
      ReadObjc3DriverEnvironmentVariable("OBJC3C_METAPROGRAMMING_CACHE_ROOT");
  if (!metaprogramming_cache_root.empty()) {
    options.metaprogramming_cache_root = metaprogramming_cache_root;
  }
}
