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
bool IsObjectPointerTypeNameConsistent(
    bool object_pointer_type_spelling,
    std::string_view object_pointer_type_name);
bool IsPointerDeclaratorDepthConsistent(
    bool has_pointer_declarator,
    unsigned pointer_declarator_depth);

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
