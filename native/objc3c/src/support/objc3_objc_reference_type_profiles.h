#pragma once

#include "ast/objc3_ast_core.h"
#include "support/objc3_objc_reference_annotation_sites.h"
#include "support/objc3_objc_reference_value_types.h"
#include "support/objc3_objc_runtime_type_surface.h"

namespace objc3c::support {

inline bool IsObjCReferenceAliasValueType(ValueType type) {
  return objc3c::support::IsObjCReferenceValueType(type);
}

}  // namespace objc3c::support
