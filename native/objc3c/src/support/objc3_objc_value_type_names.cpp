#include "support/objc3_objc_value_type_names.h"

namespace objc3c::support {

const char *ObjCValueTypeName(ValueType type) {
  switch (type) {
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
    default:
      return nullptr;
  }
}

}  // namespace objc3c::support
