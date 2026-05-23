#pragma once

#include <string>

#include "ast/objc3_ast_value_type.h"

inline constexpr const char *kObjc3ValueOptionalContractId =
    "objc3c.value_optional.contract.v1";
inline constexpr const char *kObjc3ValueOptionalAbiLayoutId =
    "objc3.value_optional.inline_presence_payload.v1";
inline constexpr const char *kObjc3ValueOptionalCanonicalSpelling =
    "Optional<T>";

struct Objc3ValueOptionalTypeDescriptor {
  bool present = false;
  std::string contract_id = kObjc3ValueOptionalContractId;
  std::string canonical_spelling = kObjc3ValueOptionalCanonicalSpelling;
  std::string payload_type_spelling;
  ValueType payload_value_type = ValueType::Unknown;
  bool payload_object_pointer = false;
  std::string payload_object_pointer_type_name;
  bool payload_generic = false;
  bool payload_nested_value_optional = false;
  bool payload_lowercase_optional_alias = false;
  std::string abi_layout_id = kObjc3ValueOptionalAbiLayoutId;
  std::string presence_field = "has_value";
  std::string payload_storage_field = "payload";
  bool source_type_admitted = false;
  bool semantic_carrier_modeled = false;
  bool runtime_execution_supported = false;
  bool lowering_supported = false;
  bool nil_to_scalar_coercion_allowed = false;
  bool nullable_pointer_conversion_allowed = false;
  bool throws_result_conversion_allowed = false;
  bool cleanup_contract_preserved = true;
  unsigned line = 1;
  unsigned column = 1;
};

inline std::string Objc3ValueOptionalPayloadSpellingFromGenericSuffix(
    const std::string &generic_suffix_text) {
  if (generic_suffix_text.size() < 2 || generic_suffix_text.front() != '<' ||
      generic_suffix_text.back() != '>') {
    return {};
  }
  return generic_suffix_text.substr(1, generic_suffix_text.size() - 2);
}

inline ValueType Objc3ValueOptionalPayloadValueType(
    const std::string &payload_type_spelling) {
  if (payload_type_spelling == "i32" ||
      payload_type_spelling == "NSInteger" ||
      payload_type_spelling == "NSUInteger") {
    return ValueType::I32;
  }
  if (payload_type_spelling == "bool" || payload_type_spelling == "BOOL") {
    return ValueType::Bool;
  }
  if (payload_type_spelling == "id") {
    return ValueType::ObjCId;
  }
  if (payload_type_spelling == "Class") {
    return ValueType::ObjCClass;
  }
  if (payload_type_spelling == "SEL") {
    return ValueType::ObjCSel;
  }
  if (payload_type_spelling == "Protocol") {
    return ValueType::ObjCProtocol;
  }
  if (payload_type_spelling == "instancetype") {
    return ValueType::ObjCInstancetype;
  }
  if (payload_type_spelling == "Text" ||
      payload_type_spelling == "Objc3Text") {
    return ValueType::TextHandle;
  }
  if (payload_type_spelling.empty()) {
    return ValueType::Unknown;
  }
  if (payload_type_spelling.back() == '*') {
    return ValueType::ObjCObjectPtr;
  }
  return ValueType::Unknown;
}

inline Objc3ValueOptionalTypeDescriptor BuildObjc3ValueOptionalDescriptor(
    const std::string &generic_suffix_text,
    unsigned line,
    unsigned column) {
  Objc3ValueOptionalTypeDescriptor descriptor;
  descriptor.present = true;
  descriptor.source_type_admitted = true;
  descriptor.semantic_carrier_modeled = true;
  descriptor.line = line;
  descriptor.column = column;
  descriptor.payload_type_spelling =
      Objc3ValueOptionalPayloadSpellingFromGenericSuffix(generic_suffix_text);
  descriptor.payload_value_type =
      Objc3ValueOptionalPayloadValueType(descriptor.payload_type_spelling);
  descriptor.payload_object_pointer =
      descriptor.payload_value_type == ValueType::ObjCObjectPtr;
  if (descriptor.payload_object_pointer) {
    descriptor.payload_object_pointer_type_name =
        descriptor.payload_type_spelling;
    while (!descriptor.payload_object_pointer_type_name.empty() &&
           descriptor.payload_object_pointer_type_name.back() == '*') {
      descriptor.payload_object_pointer_type_name.pop_back();
    }
  }
  descriptor.payload_generic =
      descriptor.payload_type_spelling.find('<') != std::string::npos ||
      descriptor.payload_type_spelling.find('>') != std::string::npos;
  descriptor.payload_nested_value_optional =
      descriptor.payload_type_spelling.rfind("Optional<", 0) == 0;
  descriptor.payload_lowercase_optional_alias =
      descriptor.payload_type_spelling.rfind("optional<", 0) == 0 ||
      descriptor.payload_type_spelling.find("<optional<") !=
          std::string::npos;
  return descriptor;
}
