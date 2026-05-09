#pragma once

#include <string_view>

#include "ast/objc3_ast_core.h"

namespace objc3c::support {

bool IsObjCReferenceAliasValueType(ValueType type);
bool IsObjCReferenceAnnotationSite(bool id_spelling,
                                   bool class_spelling,
                                   bool instancetype_spelling,
                                   bool object_pointer_type_spelling);
bool IsObjCRuntimeTypeSurface(bool id_spelling,
                              bool class_spelling,
                              bool sel_spelling,
                              bool instancetype_spelling,
                              bool object_pointer_type_spelling);

}  // namespace objc3c::support
