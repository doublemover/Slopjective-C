#include "support/objc3_value_type_names.h"

#include "support/objc3_objc_value_type_names.h"
#include "support/objc3_scalar_value_type_names.h"

namespace objc3c::support {

const char *ValueTypeName(ValueType type) {
  if (const char *name = ScalarValueTypeName(type)) {
    return name;
  }
  if (const char *name = ObjCValueTypeName(type)) {
    return name;
  }
  return "unknown";
}

}  // namespace objc3c::support
