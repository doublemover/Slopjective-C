#include <cstdint>
#include <cstdio>
#include <cstring>

#include "strict_dispatch_error_status_probe/dispatch_fixture.h"

namespace objc3c::runtime::typed_dispatch_abi_probe {

using ::objc3c::runtime::strict_dispatch_error_status_probe::MakeImageCase;
using ::objc3c::runtime::strict_dispatch_error_status_probe::RegisterCase;

inline bool ManualBoolMethod0() {
  return true;
}

inline void ManualVoidMethod0() {}

struct TypedDispatchCase {
  const char *module_name;
  const char *identity_key;
  const char *selector;
  const char *return_type_name;
  const char *return_kind_name;
  objc3_runtime_dispatch_return_kind_code return_kind;
  const void *implementation;
  int expected_value;
};

bool CommonTypedResultShapeIsValid(
    const objc3_runtime_dispatch_typed_result &result,
    const TypedDispatchCase &test_case) {
  return result.abi_version == OBJC3_RUNTIME_DISPATCH_TYPED_RESULT_ABI_VERSION &&
         result.result_size == sizeof(objc3_runtime_dispatch_typed_result) &&
         result.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK &&
         result.return_kind == test_case.return_kind &&
         result.return_kind_name != nullptr &&
         std::strcmp(result.return_kind_name, test_case.return_kind_name) == 0 &&
         result.diagnostic_code != nullptr &&
         result.diagnostic_message != nullptr &&
         result.result_contract != nullptr &&
         std::strcmp(result.result_contract, "typed-dispatch-value-result") ==
             0;
}

bool TypedValueFieldsMatch(const objc3_runtime_dispatch_typed_result &result,
                           const TypedDispatchCase &test_case) {
  const int expected_i32 =
      test_case.return_kind == OBJC3_RUNTIME_DISPATCH_RETURN_KIND_I32
          ? test_case.expected_value
          : 0;
  const int expected_bool =
      test_case.return_kind == OBJC3_RUNTIME_DISPATCH_RETURN_KIND_BOOL ? 1 : 0;
  const int expected_object =
      test_case.return_kind ==
              OBJC3_RUNTIME_DISPATCH_RETURN_KIND_OBJECT_REFERENCE
          ? test_case.expected_value
          : 0;
  const int expected_class =
      test_case.return_kind == OBJC3_RUNTIME_DISPATCH_RETURN_KIND_CLASS_REFERENCE
          ? test_case.expected_value
          : 0;
  const int expected_selector =
      test_case.return_kind ==
              OBJC3_RUNTIME_DISPATCH_RETURN_KIND_SELECTOR_REFERENCE
          ? test_case.expected_value
          : 0;
  const int expected_protocol =
      test_case.return_kind ==
              OBJC3_RUNTIME_DISPATCH_RETURN_KIND_PROTOCOL_REFERENCE
          ? test_case.expected_value
          : 0;
  return result.i32_value == expected_i32 &&
         result.bool_value == expected_bool &&
         result.object_reference == expected_object &&
         result.class_reference == expected_class &&
         result.selector_reference == expected_selector &&
         result.protocol_reference == expected_protocol;
}

bool RunTypedCase(const TypedDispatchCase &test_case) {
  auto image_case = MakeImageCase(
      test_case.module_name, test_case.identity_key, test_case.selector,
      test_case.return_type_name, 0, test_case.implementation, 1);
  if (!RegisterCase(image_case)) {
    std::fprintf(stderr, "registration failed: %s\n", test_case.selector);
    return false;
  }
  const objc3_runtime_dispatch_typed_result result =
      objc3_runtime_dispatch_typed_checked(1024, test_case.selector, 0, 0, 0,
                                           0);
  if (!CommonTypedResultShapeIsValid(result, test_case) ||
      !TypedValueFieldsMatch(result, test_case)) {
    std::fprintf(
        stderr,
        "typed case failed: %s status=%d kind=%d kind_name=%s i32=%d "
        "bool=%d object=%d class=%d selector_ref=%d protocol=%d contract=%s\n",
        test_case.selector, result.status_code, result.return_kind,
        result.return_kind_name != nullptr ? result.return_kind_name : "<null>",
        result.i32_value, result.bool_value, result.object_reference,
        result.class_reference, result.selector_reference,
        result.protocol_reference,
        result.result_contract != nullptr ? result.result_contract : "<null>");
    return false;
  }
  if (test_case.return_kind ==
      OBJC3_RUNTIME_DISPATCH_RETURN_KIND_OBJECT_REFERENCE) {
    const objc3_runtime_dispatch_i32_result i32_result =
        objc3_runtime_dispatch_i32_checked(1024, test_case.selector, 0, 0, 0,
                                           0);
    const bool i32_rejected =
        i32_result.status_code ==
            OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE &&
        i32_result.return_kind ==
            OBJC3_RUNTIME_DISPATCH_RETURN_KIND_OBJECT_REFERENCE &&
        i32_result.value == 0 && i32_result.diagnostic_code != nullptr &&
        std::strcmp(i32_result.diagnostic_code, "O3RT005") == 0 &&
        i32_result.result_contract != nullptr &&
        std::strcmp(i32_result.result_contract,
                    "typed-dispatch-strict-error-result") == 0;
    if (!i32_rejected) {
      std::fprintf(
          stderr,
          "legacy i32 mismatch check failed status=%d kind=%d value=%d "
          "code=%s contract=%s\n",
          i32_result.status_code, i32_result.return_kind, i32_result.value,
          i32_result.diagnostic_code != nullptr ? i32_result.diagnostic_code
                                                : "<null>",
          i32_result.result_contract != nullptr ? i32_result.result_contract
                                                : "<null>");
      return false;
    }
  }
  return true;
}

bool RunProbe() {
  const TypedDispatchCase cases[] = {
      {"typed-dispatch-i32",
       "typed-dispatch::i32",
       "i32Value",
       "i32",
       "i32",
       OBJC3_RUNTIME_DISPATCH_RETURN_KIND_I32,
       reinterpret_cast<const void *>(
           &::objc3c::runtime::strict_dispatch_error_status_probe::ManualMethod0),
       41},
      {"typed-dispatch-bool",
       "typed-dispatch::bool",
       "boolValue",
       "bool",
       "bool",
       OBJC3_RUNTIME_DISPATCH_RETURN_KIND_BOOL,
       reinterpret_cast<const void *>(&ManualBoolMethod0),
       1},
      {"typed-dispatch-void",
       "typed-dispatch::void",
       "voidValue",
       "void",
       "void",
       OBJC3_RUNTIME_DISPATCH_RETURN_KIND_VOID,
       reinterpret_cast<const void *>(&ManualVoidMethod0),
       0},
      {"typed-dispatch-object",
       "typed-dispatch::object",
       "objectValue",
       "id",
       "object-reference",
       OBJC3_RUNTIME_DISPATCH_RETURN_KIND_OBJECT_REFERENCE,
       reinterpret_cast<const void *>(
           &::objc3c::runtime::strict_dispatch_error_status_probe::ManualMethod0),
       41},
      {"typed-dispatch-class",
       "typed-dispatch::class",
       "classValue",
       "Class",
       "class-reference",
       OBJC3_RUNTIME_DISPATCH_RETURN_KIND_CLASS_REFERENCE,
       reinterpret_cast<const void *>(
           &::objc3c::runtime::strict_dispatch_error_status_probe::ManualMethod0),
       41},
      {"typed-dispatch-selector",
       "typed-dispatch::selector",
       "selectorValue",
       "SEL",
       "selector-reference",
       OBJC3_RUNTIME_DISPATCH_RETURN_KIND_SELECTOR_REFERENCE,
       reinterpret_cast<const void *>(
           &::objc3c::runtime::strict_dispatch_error_status_probe::ManualMethod0),
       41},
      {"typed-dispatch-protocol",
       "typed-dispatch::protocol",
       "protocolValue",
       "Protocol",
       "protocol-reference",
       OBJC3_RUNTIME_DISPATCH_RETURN_KIND_PROTOCOL_REFERENCE,
       reinterpret_cast<const void *>(
           &::objc3c::runtime::strict_dispatch_error_status_probe::ManualMethod0),
       41},
  };

  for (const TypedDispatchCase &test_case : cases) {
    if (!RunTypedCase(test_case)) {
      return false;
    }
  }
  return true;
}

}  // namespace objc3c::runtime::typed_dispatch_abi_probe

int main() {
  return objc3c::runtime::typed_dispatch_abi_probe::RunProbe() ? 0 : 1;
}
