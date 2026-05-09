#include "support/objc3_objc_reference_type_profiles.h"

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

}  // namespace objc3c::support
