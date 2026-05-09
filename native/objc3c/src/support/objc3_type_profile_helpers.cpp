#include "support/objc3_type_profile_helpers.h"

namespace objc3c::support {

bool IsObjCReferenceAliasValueType(ValueType type) {
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

bool IsObjCReferenceAnnotationSite(bool id_spelling,
                                   bool class_spelling,
                                   bool instancetype_spelling,
                                   bool object_pointer_type_spelling) {
  return id_spelling || class_spelling || instancetype_spelling ||
         object_pointer_type_spelling;
}

bool IsObjCRuntimeTypeSurface(bool id_spelling,
                              bool class_spelling,
                              bool sel_spelling,
                              bool instancetype_spelling,
                              bool object_pointer_type_spelling) {
  return id_spelling || class_spelling || sel_spelling ||
         instancetype_spelling || object_pointer_type_spelling;
}

bool IsObjectPointerTypeNameConsistent(
    bool object_pointer_type_spelling,
    std::string_view object_pointer_type_name) {
  return !object_pointer_type_spelling || !object_pointer_type_name.empty();
}

bool IsPointerDeclaratorDepthConsistent(bool has_pointer_declarator,
                                        unsigned pointer_declarator_depth) {
  return has_pointer_declarator ? pointer_declarator_depth > 0u
                                : pointer_declarator_depth == 0u;
}

}  // namespace objc3c::support
