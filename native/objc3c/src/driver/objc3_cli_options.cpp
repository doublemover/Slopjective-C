#include "driver/objc3_cli_options.h"

#include <cerrno>
#include <cctype>
#include <cstdlib>
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

}  // namespace

std::string Objc3CliUsage() {
  return "usage: objc3c-native <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] "
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

  for (int i = 2; i < argc; ++i) {
    std::string flag = argv[i];
    if (flag == "--out-dir" && i + 1 < argc) {
      options.out_dir = argv[++i];
    } else if (flag == "--emit-prefix" && i + 1 < argc) {
      options.emit_prefix = argv[++i];
    } else if (flag == "--clang" && i + 1 < argc) {
      options.clang_path = argv[++i];
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
