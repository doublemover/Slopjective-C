#include "ast/objc3_ast_value_type_queries.h"

const char *Objc3ValueTypeSpelling(ValueType type) {
  switch (type) {
    case ValueType::I32:
      return "i32";
    case ValueType::Bool:
      return "bool";
    case ValueType::Void:
      return "void";
    case ValueType::Function:
      return "function";
    case ValueType::ObjCId:
      return "id";
    case ValueType::ObjCClass:
      return "Class";
    case ValueType::ObjCSel:
      return "SEL";
    case ValueType::ObjCProtocol:
      return "Protocol";
    case ValueType::ObjCInstancetype:
      return "instancetype";
    case ValueType::ObjCObjectPtr:
      return "object-ptr";
    case ValueType::TextHandle:
      return "Text";
    case ValueType::Unknown:
    default:
      return "unknown";
  }
}

bool Objc3ValueTypeIsScalar(ValueType type) {
  return type == ValueType::I32 || type == ValueType::Bool ||
         type == ValueType::Void || type == ValueType::TextHandle;
}

bool Objc3ValueTypeIsObjectReference(ValueType type) {
  return type == ValueType::ObjCId || type == ValueType::ObjCClass ||
         type == ValueType::ObjCInstancetype ||
         type == ValueType::ObjCObjectPtr;
}

bool Objc3ValueTypeIsRuntimeMetadataReference(ValueType type) {
  return type == ValueType::ObjCClass || type == ValueType::ObjCSel ||
         type == ValueType::ObjCProtocol ||
         type == ValueType::ObjCInstancetype;
}
