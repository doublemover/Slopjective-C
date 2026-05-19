#include "driver/objc3_cli_usage.h"

#include <string>

#include "driver/objc3_cli_integer_parsers.h"

std::string Objc3CliUsage() {
  return "usage: objc3c-native <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] "
         "[--llc <path>] [--objc3-import-runtime-surface <path>]... "
         "[--objc3-enable-live-error-runtime-surface] "
         "[-fobjc-version=<N>] [-fobjc-arc] [-fno-objc-arc] "
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
