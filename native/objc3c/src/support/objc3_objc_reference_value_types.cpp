#include "support/objc3_objc_reference_value_types.h"

namespace objc3c::support {

bool IsObjCReferenceValueType(ValueType type) {
  switch (type) {
    case ValueType::ObjCId:
    case ValueType::ObjCClass:
    case ValueType::ObjCSel:
    case ValueType::ObjCProtocol:
    case ValueType::ObjCInstancetype:
    case ValueType::ObjCObjectPtr:
      return true;
    default:
      return false;
  }
}

}  // namespace objc3c::support
