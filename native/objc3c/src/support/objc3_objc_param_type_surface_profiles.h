#pragma once

#include "support/objc3_objc_reference_type_profiles.h"

namespace objc3c::support {

template <typename ParamLike>
inline bool SupportsObjCParamTypeSuffix(const ParamLike &param) {
  return IsObjCReferenceAnnotationSite(
      param.id_spelling, param.class_spelling, param.instancetype_spelling,
      param.object_pointer_type_spelling);
}

template <typename ParamLike>
inline bool SupportsObjCParamPointerDeclarator(const ParamLike &param) {
  return SupportsObjCParamTypeSuffix(param);
}

}  // namespace objc3c::support
