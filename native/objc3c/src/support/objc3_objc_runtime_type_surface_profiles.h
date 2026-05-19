#pragma once

#include "support/objc3_objc_reference_type_profiles.h"

namespace objc3c::support {

template <typename CallableLike>
inline bool HasObjCRuntimeReturnTypeSurface(const CallableLike &callable) {
  return IsObjCRuntimeTypeSurface(
      callable.return_id_spelling, callable.return_class_spelling,
      callable.return_sel_spelling, callable.return_instancetype_spelling,
      callable.return_object_pointer_type_spelling);
}

template <typename ParamLike>
inline bool HasObjCRuntimeParamTypeSurface(const ParamLike &param) {
  return IsObjCRuntimeTypeSurface(
      param.id_spelling, param.class_spelling, param.sel_spelling,
      param.instancetype_spelling, param.object_pointer_type_spelling);
}

}  // namespace objc3c::support
