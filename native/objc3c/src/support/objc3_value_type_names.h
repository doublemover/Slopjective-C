#pragma once

#include "ast/objc3_ast_core.h"

namespace objc3c::support {

inline const char *ValueTypeName(ValueType type) {
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
      return "object-pointer";
    case ValueType::Unknown:
    default:
      return "unknown";
  }
}

}  // namespace objc3c::support
