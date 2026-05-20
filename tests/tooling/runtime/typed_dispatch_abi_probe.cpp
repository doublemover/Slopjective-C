#include <cstdint>
#include <cstdio>
#include <cstring>

#include "strict_dispatch_error_status_probe/dispatch_fixture.h"

namespace objc3c::runtime::typed_dispatch_abi_probe {

using ::objc3c::runtime::strict_dispatch_error_status_probe::MakeImageCase;
using ::objc3c::runtime::strict_dispatch_error_status_probe::ManualImageCase;
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

ManualImageCase &MakePersistentImageCase(
    const char *module_name, const char *identity_key, const char *selector,
    const char *return_type_name, std::uint64_t parameter_count,
    const void *implementation, std::uint64_t method_header_count) {
  auto *image_case = new ManualImageCase(MakeImageCase(
      module_name, identity_key, selector, return_type_name, parameter_count,
      implementation, method_header_count));
  return *image_case;
}

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

bool TypedValueFieldsAreZero(const objc3_runtime_dispatch_typed_result &result) {
  return result.i32_value == 0 && result.bool_value == 0 &&
         result.object_reference == 0 && result.class_reference == 0 &&
         result.selector_reference == 0 && result.protocol_reference == 0;
}

bool TypedStatusShapeIsValid(
    const objc3_runtime_dispatch_typed_result &result,
    objc3_runtime_dispatch_status_code expected_status,
    objc3_runtime_dispatch_return_kind_code expected_return_kind,
    const char *expected_return_kind_name, const char *expected_code,
    const char *expected_contract) {
  return result.abi_version == OBJC3_RUNTIME_DISPATCH_TYPED_RESULT_ABI_VERSION &&
         result.result_size == sizeof(objc3_runtime_dispatch_typed_result) &&
         result.status_code == expected_status &&
         result.return_kind == expected_return_kind &&
         result.return_kind_name != nullptr &&
         std::strcmp(result.return_kind_name, expected_return_kind_name) == 0 &&
         TypedValueFieldsAreZero(result) && result.diagnostic_code != nullptr &&
         std::strcmp(result.diagnostic_code, expected_code) == 0 &&
         result.diagnostic_message != nullptr &&
         result.diagnostic_message[0] != '\0' &&
         result.result_contract != nullptr &&
         std::strcmp(result.result_contract, expected_contract) == 0;
}

bool I32StatusShapeIsValid(
    const objc3_runtime_dispatch_i32_result &result,
    objc3_runtime_dispatch_status_code expected_status,
    objc3_runtime_dispatch_return_kind_code expected_return_kind,
    const char *expected_code, const char *expected_contract) {
  return result.abi_version == OBJC3_RUNTIME_DISPATCH_I32_RESULT_ABI_VERSION &&
         result.result_size == sizeof(objc3_runtime_dispatch_i32_result) &&
         result.status_code == expected_status &&
         result.return_kind == expected_return_kind && result.value == 0 &&
         result.diagnostic_code != nullptr &&
         std::strcmp(result.diagnostic_code, expected_code) == 0 &&
         result.diagnostic_message != nullptr &&
         result.diagnostic_message[0] != '\0' &&
         result.result_contract != nullptr &&
         std::strcmp(result.result_contract, expected_contract) == 0;
}

bool I32ValueShapeIsValid(const objc3_runtime_dispatch_i32_result &result,
                          int expected_value) {
  return result.abi_version == OBJC3_RUNTIME_DISPATCH_I32_RESULT_ABI_VERSION &&
         result.result_size == sizeof(objc3_runtime_dispatch_i32_result) &&
         result.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK &&
         result.return_kind == OBJC3_RUNTIME_DISPATCH_RETURN_KIND_I32 &&
         result.value == expected_value &&
         result.diagnostic_code != nullptr &&
         result.diagnostic_code[0] == '\0' &&
         result.diagnostic_message != nullptr &&
         result.diagnostic_message[0] == '\0' &&
         result.result_contract != nullptr &&
         std::strcmp(result.result_contract, "typed-dispatch-value-result") ==
             0;
}

bool I32ProjectionIsRejected(const TypedDispatchCase &test_case) {
  if (test_case.return_kind == OBJC3_RUNTIME_DISPATCH_RETURN_KIND_I32) {
    const objc3_runtime_dispatch_i32_result i32_result =
        objc3_runtime_dispatch_i32_checked(1024, test_case.selector, 0, 0, 0,
                                           0);
    const bool i32_valid =
        I32ValueShapeIsValid(i32_result, test_case.expected_value);
    if (!i32_valid) {
      std::fprintf(
          stderr,
          "legacy i32 success check failed selector=%s status=%d kind=%d "
          "value=%d code=%s contract=%s\n",
          test_case.selector, i32_result.status_code, i32_result.return_kind,
          i32_result.value,
          i32_result.diagnostic_code != nullptr ? i32_result.diagnostic_code
                                                : "<null>",
          i32_result.result_contract != nullptr ? i32_result.result_contract
                                                : "<null>");
    }
    return i32_valid;
  }
  const objc3_runtime_dispatch_i32_result i32_result =
      objc3_runtime_dispatch_i32_checked(1024, test_case.selector, 0, 0, 0, 0);
  const bool i32_rejected = I32StatusShapeIsValid(
      i32_result, OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE,
      test_case.return_kind, "O3RT005",
      "typed-dispatch-strict-error-result");
  if (!i32_rejected) {
    std::fprintf(
        stderr,
        "legacy i32 mismatch check failed selector=%s status=%d kind=%d "
        "value=%d code=%s contract=%s\n",
        test_case.selector, i32_result.status_code, i32_result.return_kind,
        i32_result.value,
        i32_result.diagnostic_code != nullptr ? i32_result.diagnostic_code
                                              : "<null>",
        i32_result.result_contract != nullptr ? i32_result.result_contract
                                              : "<null>");
  }
  return i32_rejected;
}

bool RunTypedCase(const TypedDispatchCase &test_case) {
  ManualImageCase &image_case = MakePersistentImageCase(
      test_case.module_name, test_case.identity_key, test_case.selector,
      test_case.return_type_name, 0, test_case.implementation, 1);
  if (!RegisterCase(image_case)) {
    std::fprintf(stderr, "registration failed: %s\n", test_case.selector);
    return false;
  }
  const objc3_runtime_dispatch_typed_result result =
      objc3_runtime_dispatch_typed_checked(1024, test_case.selector, 0, 0, 0,
                                           0);
  const objc3_runtime_dispatch_typed_result cached_result =
      objc3_runtime_dispatch_typed_checked(1024, test_case.selector, 0, 0, 0,
                                           0);
  if (!CommonTypedResultShapeIsValid(result, test_case) ||
      !TypedValueFieldsMatch(result, test_case) ||
      !CommonTypedResultShapeIsValid(cached_result, test_case) ||
      !TypedValueFieldsMatch(cached_result, test_case)) {
    std::fprintf(
        stderr,
        "typed case failed: %s first_status=%d cached_status=%d first_kind=%d "
        "cached_kind=%d kind_name=%s cached_kind_name=%s i32=%d cached_i32=%d "
        "bool=%d cached_bool=%d object=%d cached_object=%d class=%d "
        "cached_class=%d selector_ref=%d cached_selector_ref=%d protocol=%d "
        "cached_protocol=%d contract=%s cached_contract=%s\n",
        test_case.selector, result.status_code, cached_result.status_code,
        result.return_kind, cached_result.return_kind,
        result.return_kind_name != nullptr ? result.return_kind_name : "<null>",
        cached_result.return_kind_name != nullptr
            ? cached_result.return_kind_name
            : "<null>",
        result.i32_value, cached_result.i32_value, result.bool_value,
        cached_result.bool_value, result.object_reference,
        cached_result.object_reference, result.class_reference,
        cached_result.class_reference, result.selector_reference,
        cached_result.selector_reference, result.protocol_reference,
        cached_result.protocol_reference,
        result.result_contract != nullptr ? result.result_contract : "<null>",
        cached_result.result_contract != nullptr ? cached_result.result_contract
                                                : "<null>");
    return false;
  }
  const bool projection_rejected = I32ProjectionIsRejected(test_case);
  objc3_runtime_reset_for_testing();
  return projection_rejected;
}

bool RunTypedNilAndUnknownSelectorCases() {
  ManualImageCase &image_case = MakePersistentImageCase(
      "typed-dispatch-negative-base", "typed-dispatch::negative-base",
      "knownValue", "i32", 0,
      reinterpret_cast<const void *>(
          &::objc3c::runtime::strict_dispatch_error_status_probe::ManualMethod0),
      1);
  if (!RegisterCase(image_case)) {
    std::fprintf(stderr, "registration failed: typed negative base\n");
    return false;
  }
  const objc3_runtime_dispatch_typed_result nil_typed =
      objc3_runtime_dispatch_typed_checked(0, "knownValue", 0, 0, 0, 0);
  const objc3_runtime_dispatch_i32_result nil_i32 =
      objc3_runtime_dispatch_i32_checked(0, "knownValue", 0, 0, 0, 0);
  if (!TypedStatusShapeIsValid(
          nil_typed, OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER,
          OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED, "unsupported",
          "O3RT008", "typed-dispatch-value-result") ||
      !I32StatusShapeIsValid(nil_i32,
                             OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER,
                             OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED,
                             "O3RT008", "typed-dispatch-value-result")) {
    std::fprintf(stderr, "typed nil receiver case failed\n");
    objc3_runtime_reset_for_testing();
    return false;
  }
  const objc3_runtime_dispatch_typed_result unknown_typed =
      objc3_runtime_dispatch_typed_checked(1024, "missingValue", 0, 0, 0, 0);
  const objc3_runtime_dispatch_i32_result unknown_i32 =
      objc3_runtime_dispatch_i32_checked(1024, "missingValue", 0, 0, 0, 0);
  if (!TypedStatusShapeIsValid(
          unknown_typed, OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR,
          OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED, "unsupported",
          "O3RT001", "typed-dispatch-strict-error-result") ||
      !I32StatusShapeIsValid(
          unknown_i32, OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR,
          OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED, "O3RT001",
          "typed-dispatch-strict-error-result")) {
    std::fprintf(stderr, "typed unknown selector case failed\n");
    objc3_runtime_reset_for_testing();
    return false;
  }
  objc3_runtime_reset_for_testing();
  return true;
}

bool RunTypedUnsupportedReturnCase() {
  ManualImageCase &image_case = MakePersistentImageCase(
      "typed-dispatch-unsupported-return",
      "typed-dispatch::unsupported-return", "doubleValue", "double", 0,
      reinterpret_cast<const void *>(
          &::objc3c::runtime::strict_dispatch_error_status_probe::ManualMethod0),
      1);
  if (!RegisterCase(image_case)) {
    std::fprintf(stderr, "registration failed: typed unsupported return\n");
    return false;
  }
  const objc3_runtime_dispatch_typed_result typed_result =
      objc3_runtime_dispatch_typed_checked(1024, "doubleValue", 0, 0, 0, 0);
  const objc3_runtime_dispatch_typed_result cached_typed_result =
      objc3_runtime_dispatch_typed_checked(1024, "doubleValue", 0, 0, 0, 0);
  const objc3_runtime_dispatch_i32_result i32_result =
      objc3_runtime_dispatch_i32_checked(1024, "doubleValue", 0, 0, 0, 0);
  const bool valid =
      TypedStatusShapeIsValid(
          typed_result, OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE,
          OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED, "unsupported",
          "O3RT005", "typed-dispatch-strict-error-result") &&
      TypedStatusShapeIsValid(
          cached_typed_result,
          OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE,
          OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED, "unsupported",
          "O3RT005", "typed-dispatch-strict-error-result") &&
      I32StatusShapeIsValid(
          i32_result, OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE,
          OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED, "O3RT005",
          "typed-dispatch-strict-error-result");
  objc3_runtime_reset_for_testing();
  return valid;
}

bool RunTypedUnsupportedArgumentCase() {
  ManualImageCase &image_case = MakePersistentImageCase(
      "typed-dispatch-unsupported-arguments",
      "typed-dispatch::unsupported-arguments", "tooMany:args:for:i32:path:",
      "i32", 5,
      reinterpret_cast<const void *>(
          &::objc3c::runtime::strict_dispatch_error_status_probe::ManualMethod5),
      1);
  if (!RegisterCase(image_case)) {
    std::fprintf(stderr, "registration failed: typed unsupported args\n");
    return false;
  }
  const objc3_runtime_dispatch_typed_result typed_result =
      objc3_runtime_dispatch_typed_checked(
          1024, "tooMany:args:for:i32:path:", 1, 2, 3, 4);
  const objc3_runtime_dispatch_typed_result cached_typed_result =
      objc3_runtime_dispatch_typed_checked(
          1024, "tooMany:args:for:i32:path:", 1, 2, 3, 4);
  const objc3_runtime_dispatch_i32_result i32_result =
      objc3_runtime_dispatch_i32_checked(
          1024, "tooMany:args:for:i32:path:", 1, 2, 3, 4);
  const bool valid =
      TypedStatusShapeIsValid(
          typed_result,
          OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT,
          OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED, "unsupported",
          "O3RT006", "typed-dispatch-strict-error-result") &&
      TypedStatusShapeIsValid(
          cached_typed_result,
          OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT,
          OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED, "unsupported",
          "O3RT006", "typed-dispatch-strict-error-result") &&
      I32StatusShapeIsValid(
          i32_result, OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT,
          OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED, "O3RT006",
          "typed-dispatch-strict-error-result");
  objc3_runtime_reset_for_testing();
  return valid;
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
  return RunTypedNilAndUnknownSelectorCases() &&
         RunTypedUnsupportedReturnCase() && RunTypedUnsupportedArgumentCase();
}

}  // namespace objc3c::runtime::typed_dispatch_abi_probe

int main() {
  return objc3c::runtime::typed_dispatch_abi_probe::RunProbe() ? 0 : 1;
}
