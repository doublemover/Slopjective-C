#include "ir/objc3_ir_type_model.h"

const char *LLVMScalarType(ValueType type) {
  if (type == ValueType::Bool) {
    return "i1";
  }
  if (type == ValueType::Void) {
    return "void";
  }
  return "i32";
}

ValueType RuntimeMetadataValueType(const std::string &type_name) {
  if (type_name == "void") {
    return ValueType::Void;
  }
  if (type_name == "bool") {
    return ValueType::Bool;
  }
  if (type_name.empty()) {
    return ValueType::Unknown;
  }
  return ValueType::I32;
}
