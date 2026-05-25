#include "ir/objc3_ir_type_model.h"

const char *LLVMScalarType(ValueType type) {
  if (type == ValueType::Bool) {
    return "i1";
  }
  if (type == ValueType::Void) {
    return "void";
  }
  if (type == ValueType::Optional) {
    return "i64";
  }
  return "i32";
}

unsigned LLVMScalarAlignment(ValueType type) {
  if (type == ValueType::Optional) {
    return 8u;
  }
  if (type == ValueType::Bool) {
    return 1u;
  }
  return 4u;
}

const char *LLVMLocalStorageType(ValueType type) {
  if (type == ValueType::Optional) {
    return "i64";
  }
  return "i32";
}

unsigned LLVMLocalStorageAlignment(ValueType type) {
  if (type == ValueType::Optional) {
    return 8u;
  }
  return 4u;
}

const char *LLVMScalarTypeForValueOptionalCarrier(
    ValueType type,
    const Objc3IRValueOptionalCarrierMetadata &carrier) {
  if (type == ValueType::Optional) {
    return Objc3IRValueOptionalCarrierLLVMType(
        Objc3IRValueOptionalCarrierKindFor(carrier));
  }
  return LLVMScalarType(type);
}

unsigned LLVMScalarAlignmentForValueOptionalCarrier(
    ValueType type,
    const Objc3IRValueOptionalCarrierMetadata &carrier) {
  if (type == ValueType::Optional) {
    return Objc3IRValueOptionalCarrierLLVMAlignment(
        Objc3IRValueOptionalCarrierKindFor(carrier));
  }
  return LLVMScalarAlignment(type);
}

const char *LLVMLocalStorageTypeForValueOptionalCarrier(
    ValueType type, Objc3IRValueOptionalCarrierKind carrier) {
  if (type == ValueType::Optional) {
    return Objc3IRValueOptionalCarrierLLVMType(carrier);
  }
  return LLVMLocalStorageType(type);
}

unsigned LLVMLocalStorageAlignmentForValueOptionalCarrier(
    ValueType type, Objc3IRValueOptionalCarrierKind carrier) {
  if (type == ValueType::Optional) {
    return Objc3IRValueOptionalCarrierLLVMAlignment(carrier);
  }
  return LLVMLocalStorageAlignment(type);
}

const char *LLVMZeroValueForValueOptionalCarrier(
    ValueType type, Objc3IRValueOptionalCarrierKind carrier) {
  if (type == ValueType::Optional) {
    return Objc3IRValueOptionalCarrierZeroValue(carrier);
  }
  return "0";
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
