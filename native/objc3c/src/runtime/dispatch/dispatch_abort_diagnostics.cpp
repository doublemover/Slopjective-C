#include "runtime/dispatch/dispatch_abort_diagnostics.h"

#include <cstdio>
#include <cstdlib>

namespace objc3c::runtime {
namespace {

const char *DispatchAbortMessage(
    const objc3_runtime_dispatch_i32_result &result) {
  return result.diagnostic_message != nullptr &&
                 result.diagnostic_message[0] != '\0'
             ? result.diagnostic_message
             : "runtime dispatch failed: missing diagnostic message";
}

const char *DispatchAbortCode(const objc3_runtime_dispatch_i32_result &result) {
  return result.diagnostic_code != nullptr && result.diagnostic_code[0] != '\0'
             ? result.diagnostic_code
             : "O3RT000";
}

}  // namespace

[[noreturn]] void AbortRuntimeDispatchFailure(
    const objc3_runtime_dispatch_i32_result &result) {
  std::fprintf(stderr, "%s [%s]\n", DispatchAbortMessage(result),
               DispatchAbortCode(result));
  std::abort();
}

}  // namespace objc3c::runtime
