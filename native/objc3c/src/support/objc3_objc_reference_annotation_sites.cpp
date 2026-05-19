#include "support/objc3_objc_reference_annotation_sites.h"

namespace objc3c::support {

bool IsObjCReferenceAnnotationSite(bool id_spelling,
                                   bool class_spelling,
                                   bool instancetype_spelling,
                                   bool object_pointer_type_spelling) {
  return id_spelling || class_spelling || instancetype_spelling ||
         object_pointer_type_spelling;
}

}  // namespace objc3c::support
