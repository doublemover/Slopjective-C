#include "ir/objc3_ir_type_model.h"

const char *LLVMScalarType(ValueType type) {
  if (type == ValueType::Bool) {
    return "i1";
  }
  if (type == ValueType::Void) {
    return "void";
  }
  if (type == ValueType::Optional) {
    return "%objc3.value_optional";
  }
  return "i32";
}

ValueType RuntimeMetadataValueType(const std::string &type_name) {
  if (type_name == "i32") {
    return ValueType::I32;
  }
  if (type_name == "void") {
    return ValueType::Void;
  }
  if (type_name == "bool") {
    return ValueType::Bool;
  }
  if (type_name == "id") {
    return ValueType::ObjCId;
  }
  if (type_name == "Class") {
    return ValueType::ObjCClass;
  }
  if (type_name == "SEL") {
    return ValueType::ObjCSel;
  }
  if (type_name == "Protocol") {
    return ValueType::ObjCProtocol;
  }
  if (type_name == "instancetype") {
    return ValueType::ObjCInstancetype;
  }
  if (type_name == "object-pointer") {
    return ValueType::ObjCObjectPtr;
  }
  if (type_name == "Text" || type_name == "Objc3Text" ||
      type_name == "text-handle") {
    return ValueType::TextHandle;
  }
  if (type_name == "optional" || type_name == "Optional") {
    return ValueType::Optional;
  }
  if (type_name == "unknown") {
    return ValueType::Unknown;
  }
  if (type_name.empty()) {
    return ValueType::Unknown;
  }
  return ValueType::I32;
}
