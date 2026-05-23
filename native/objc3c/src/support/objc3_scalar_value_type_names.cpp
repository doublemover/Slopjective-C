#include "support/objc3_scalar_value_type_names.h"

namespace objc3c::support {

const char *ScalarValueTypeName(ValueType type) {
  switch (type) {
    case ValueType::I32:
      return "i32";
    case ValueType::Bool:
      return "bool";
    case ValueType::Void:
      return "void";
    case ValueType::Function:
      return "function";
    case ValueType::Optional:
      return "optional";
    case ValueType::TextHandle:
      return "Text";
    default:
      return nullptr;
  }
}

}  // namespace objc3c::support
