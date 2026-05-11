#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_STRICT_ERROR_SCENARIOS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_STRICT_ERROR_SCENARIOS_H_

#include "dispatch_fixture.h"

namespace objc3c::runtime::strict_dispatch_error_status_probe {

struct DispatchArguments {
  int arg0;
  int arg1;
  int arg2;
  int arg3;
};

struct StrictDispatchScenario {
  const char *module_name;
  const char *identity_key;
  const char *selector;
  const char *return_type_name;
  std::uint64_t parameter_count;
  const void *implementation;
  std::uint64_t method_header_count;
  DispatchArguments arguments;
  int expected_status;
  const char *expected_error_code;
  const char *expected_error_message;
  int exit_code_base;
};

struct StrictDispatchScenarioList {
  const StrictDispatchScenario *items;
  std::size_t count;
};

inline StrictDispatchScenario UnsupportedReturnScenario() {
  return {"strict-dispatch-unsupported-return",
          "strict-dispatch::unsupported-return",
          "objectValue",
          "double",
          0,
          reinterpret_cast<const void *>(&ManualMethod0),
          1,
          {0, 0, 0, 0},
          OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE,
          "O3RT005",
          "runtime dispatch failed: unsupported return type",
          10};
}

inline StrictDispatchScenario UnsupportedArgumentsScenario() {
  return {"strict-dispatch-unsupported-arguments",
          "strict-dispatch::unsupported-arguments",
          "tooMany:args:for:i32:path:",
          "i32",
          5,
          reinterpret_cast<const void *>(&ManualMethod5),
          1,
          {1, 2, 3, 4},
          OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT,
          "O3RT006",
          "runtime dispatch failed: unsupported argument layout",
          20};
}

inline StrictDispatchScenario MalformedMetadataScenario() {
  return {"strict-dispatch-malformed-metadata",
          "strict-dispatch::malformed-metadata",
          "malformedValue",
          "i32",
          0,
          reinterpret_cast<const void *>(&ManualMethod0),
          2,
          {0, 0, 0, 0},
          OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA,
          "O3RT004",
          "runtime dispatch failed: malformed metadata",
          30};
}

inline StrictDispatchScenarioList StrictDispatchScenarios() {
  static const StrictDispatchScenario scenarios[] = {
      UnsupportedReturnScenario(),
      UnsupportedArgumentsScenario(),
      MalformedMetadataScenario(),
  };
  return {scenarios, sizeof(scenarios) / sizeof(scenarios[0])};
}

inline objc3_runtime_dispatch_i32_result InvokeScenario(
    const StrictDispatchScenario &scenario) {
  return objc3_runtime_dispatch_i32_checked(
      1024, scenario.selector, scenario.arguments.arg0, scenario.arguments.arg1,
      scenario.arguments.arg2, scenario.arguments.arg3);
}

}  // namespace objc3c::runtime::strict_dispatch_error_status_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_STRICT_ERROR_SCENARIOS_H_
