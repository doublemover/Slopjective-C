#pragma once

#include "support/objc3_objc_reference_type_profiles.h"

namespace objc3c::support {

template <typename PropertyLike>
inline bool SupportsObjCPropertyTypeSuffix(const PropertyLike &property) {
  return IsObjCReferenceAnnotationSite(
      property.id_spelling, property.class_spelling,
      property.instancetype_spelling, property.object_pointer_type_spelling);
}

template <typename PropertyLike>
inline bool SupportsObjCPropertyPointerDeclarator(
    const PropertyLike &property) {
  return SupportsObjCPropertyTypeSuffix(property);
}

}  // namespace objc3c::support
