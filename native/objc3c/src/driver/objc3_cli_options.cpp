#include "driver/objc3_cli_options.h"

#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <string>

namespace {

constexpr std::size_t kMaxMessageSendArgs = 16;

bool IsRuntimeDispatchSymbolStart(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_' || c == '$' || c == '.';
}

bool IsRuntimeDispatchSymbolBody(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_' || c == '$' || c == '.';
}

bool IsValidRuntimeDispatchSymbol(const std::string &symbol) {
  if (symbol.empty() || !IsRuntimeDispatchSymbolStart(symbol[0])) {
    return false;
  }
  for (std::size_t i = 1; i < symbol.size(); ++i) {
    if (!IsRuntimeDispatchSymbolBody(symbol[i])) {
      return false;
    }
  }
  return true;
}

bool ParseIrObjectBackend(const std::string &value, Objc3IrObjectBackend &backend) {
  if (value == "clang") {
    backend = Objc3IrObjectBackend::kClang;
    return true;
  }
  if (value == "llvm-direct") {
    backend = Objc3IrObjectBackend::kLLVMDirect;
    return true;
  }
  return false;
}

std::string ReadEnvironmentVariable(const char *name) {
#if defined(_WIN32)
  char *value = nullptr;
  std::size_t value_length = 0;
  if (_dupenv_s(&value, &value_length, name) != 0 || value == nullptr || value_length == 0) {
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

std::filesystem::path DefaultLlcPath() {
#if defined(_WIN32)
  constexpr const char *llc_name = "llc.exe";
#else
  constexpr const char *llc_name = "llc";
#endif
  const std::string llvm_root = ReadEnvironmentVariable("LLVM_ROOT");
  if (!llvm_root.empty()) {
    return std::filesystem::path(llvm_root) / "bin" / llc_name;
  }
#if defined(_WIN32)
  const std::filesystem::path standard_path = std::filesystem::path("C:\\Program Files\\LLVM\\bin\\llc.exe");
  if (std::filesystem::exists(standard_path)) {
    return standard_path;
  }
#endif
  return std::filesystem::path(llc_name);
}

}  // namespace

std::string Objc3CliUsage() {
  return "usage: objc3c-native <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] "
         "[--llc <path>] "
         "[--objc3-ir-object-backend <clang|llvm-direct>] "
         "[--llvm-capabilities-summary <path>] [--objc3-route-backend-from-capabilities] "
         "[--objc3-max-message-args <0-" +
         std::to_string(kMaxMessageSendArgs) +
         ">] [--objc3-runtime-dispatch-symbol <symbol>]";
}

bool ParseObjc3CliOptions(int argc, char **argv, Objc3CliOptions &options, std::string &error) {
  if (argc < 2) {
    error = Objc3CliUsage();
    return false;
  }

  options = Objc3CliOptions{};
  options.input = argv[1];
  options.llc_path = DefaultLlcPath();

  for (int i = 2; i < argc; ++i) {
    std::string flag = argv[i];
    if (flag == "--out-dir" && i + 1 < argc) {
      options.out_dir = argv[++i];
    } else if (flag == "--emit-prefix" && i + 1 < argc) {
      options.emit_prefix = argv[++i];
    } else if (flag == "--clang" && i + 1 < argc) {
      options.clang_path = argv[++i];
      options.clang_path_explicit = true;
    } else if (flag == "--llc" && i + 1 < argc) {
      options.llc_path = argv[++i];
      options.llc_path_explicit = true;
    } else if (flag == "--objc3-ir-object-backend" && i + 1 < argc) {
      const std::string backend = argv[++i];
      if (!ParseIrObjectBackend(backend, options.ir_object_backend)) {
        error = "invalid --objc3-ir-object-backend (expected clang|llvm-direct): " + backend;
        return false;
      }
    } else if (flag == "--llvm-capabilities-summary" && i + 1 < argc) {
      options.llvm_capabilities_summary = argv[++i];
    } else if (flag == "--objc3-route-backend-from-capabilities") {
      options.route_backend_from_capabilities = true;
    } else if (flag == "--objc3-max-message-args" && i + 1 < argc) {
      const std::string value = argv[++i];
      errno = 0;
      char *end = nullptr;
      const unsigned long parsed = std::strtoul(value.c_str(), &end, 10);
      if (value.empty() || end == value.c_str() || *end != '\0' || errno == ERANGE ||
          parsed > kMaxMessageSendArgs) {
        error = "invalid --objc3-max-message-args (expected integer 0-" +
                std::to_string(kMaxMessageSendArgs) + "): " + value;
        return false;
      }
      options.max_message_send_args = static_cast<std::size_t>(parsed);
    } else if (flag == "--objc3-runtime-dispatch-symbol" && i + 1 < argc) {
      const std::string symbol = argv[++i];
      if (!IsValidRuntimeDispatchSymbol(symbol)) {
        error = "invalid --objc3-runtime-dispatch-symbol (expected [A-Za-z_.$][A-Za-z0-9_.$]*): " + symbol;
        return false;
      }
      options.runtime_dispatch_symbol = symbol;
    } else {
      error = "unknown arg: " + flag;
      return false;
    }
  }

  return true;
}
