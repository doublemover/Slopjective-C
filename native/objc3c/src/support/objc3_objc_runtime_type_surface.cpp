#include "support/objc3_objc_runtime_type_surface.h"

namespace objc3c::support {

bool IsObjCRuntimeTypeSurface(bool id_spelling,
                              bool class_spelling,
                              bool sel_spelling,
                              bool instancetype_spelling,
                              bool object_pointer_type_spelling) {
  return id_spelling || class_spelling || sel_spelling ||
         instancetype_spelling || object_pointer_type_spelling;
}

}  // namespace objc3c::support
