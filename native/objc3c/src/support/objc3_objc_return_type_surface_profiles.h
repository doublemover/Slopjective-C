#pragma once

#include "support/objc3_objc_reference_type_profiles.h"

namespace objc3c::support {

template <typename CallableLike>
inline bool SupportsObjCReturnTypeSuffix(const CallableLike &callable) {
  return IsObjCReferenceAnnotationSite(
      callable.return_id_spelling, callable.return_class_spelling,
      callable.return_instancetype_spelling,
      callable.return_object_pointer_type_spelling);
}

template <typename CallableLike>
inline bool SupportsObjCReturnPointerDeclarator(const CallableLike &callable) {
  return SupportsObjCReturnTypeSuffix(callable);
}

}  // namespace objc3c::support
