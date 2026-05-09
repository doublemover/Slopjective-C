#include "driver/objc3_cli_value_parsers.h"

#include <cerrno>
#include <cstdlib>
#include <limits>

#include "support/objc3_ir_object_backend_token.h"

bool ParseObjc3CliIrObjectBackend(const std::string &value,
                                  Objc3IrObjectBackend &backend) {
  objc3c::support::IrObjectBackendToken token;
  if (!objc3c::support::ParseIrObjectBackendToken(value, token)) {
    return false;
  }
  if (token == objc3c::support::IrObjectBackendToken::Clang) {
    backend = Objc3IrObjectBackend::kClang;
    return true;
  }
  backend = Objc3IrObjectBackend::kLLVMDirect;
  return true;
}

bool ParseObjc3ConformanceProfile(const std::string &value,
                                  Objc3ConformanceProfile &profile) {
  if (value == "core") {
    profile = Objc3ConformanceProfile::kCore;
    return true;
  }
  if (value == "strict") {
    profile = Objc3ConformanceProfile::kStrict;
    return true;
  }
  if (value == "strict-concurrency") {
    profile = Objc3ConformanceProfile::kStrictConcurrency;
    return true;
  }
  if (value == "strict-system") {
    profile = Objc3ConformanceProfile::kStrictSystem;
    return true;
  }
  return false;
}

bool ParseObjc3LanguageVersion(const std::string &value,
                               std::uint32_t &version) {
  errno = 0;
  char *end = nullptr;
  const unsigned long parsed = std::strtoul(value.c_str(), &end, 10);
  if (value.empty() || end == value.c_str() || *end != '\0' ||
      errno == ERANGE ||
      parsed > std::numeric_limits<std::uint32_t>::max()) {
    return false;
  }
  version = static_cast<std::uint32_t>(parsed);
  return true;
}

bool ParseObjc3PositiveOrdinal(const std::string &value,
                               std::uint64_t &ordinal) {
  errno = 0;
  char *end = nullptr;
  const unsigned long long parsed = std::strtoull(value.c_str(), &end, 10);
  if (value.empty() || end == value.c_str() || *end != '\0' ||
      errno == ERANGE || parsed == 0 ||
      parsed > std::numeric_limits<std::uint64_t>::max()) {
    return false;
  }
  ordinal = static_cast<std::uint64_t>(parsed);
  return true;
}

bool ParseObjc3MessageSendArgLimit(const std::string &value,
                                   std::size_t &max_args) {
  errno = 0;
  char *end = nullptr;
  const unsigned long parsed = std::strtoul(value.c_str(), &end, 10);
  if (value.empty() || end == value.c_str() || *end != '\0' ||
      errno == ERANGE || parsed > kObjc3CliMaxMessageSendArgs) {
    return false;
  }
  max_args = static_cast<std::size_t>(parsed);
  return true;
}

std::string ConformanceProfileName(Objc3ConformanceProfile profile) {
  switch (profile) {
    case Objc3ConformanceProfile::kCore:
      return "core";
    case Objc3ConformanceProfile::kStrict:
      return "strict";
    case Objc3ConformanceProfile::kStrictConcurrency:
      return "strict-concurrency";
    case Objc3ConformanceProfile::kStrictSystem:
      return "strict-system";
    default:
      return "core";
  }
}

std::string Objc3CliUsage() {
  return "usage: objc3c-native <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] "
         "[--llc <path>] [--objc3-import-runtime-surface <path>]... "
         "[-fobjc-version=<N>] [--objc3-language-version <N>] "
         "[-fobjc-arc] [-fno-objc-arc] "
         "[--objc3-conformance-profile <core|strict|strict-concurrency|strict-system>] "
         "[--emit-objc3-conformance] [--emit-objc3-conformance-format <json>] "
         "[--validate-objc3-conformance <report.json>] "
         "[--objc3-bootstrap-registration-order-ordinal <positive-int>] "
         "[--objc3-metaprogramming-cache-root <dir>] "
         "[--objc3-ir-object-backend <clang|llvm-direct>] "
         "[--llvm-capabilities-summary <path>] [--objc3-route-backend-from-capabilities] "
         "[--objc3-max-message-args <0-" +
         std::to_string(kObjc3CliMaxMessageSendArgs) +
         ">] [--objc3-runtime-dispatch-symbol <symbol>]";
}
