#pragma once

#include "ast/objc3_ast_core.h"

#include <algorithm>

template <typename CallableDeclT>
bool HasInteropForeignOrImportAnnotations(const CallableDeclT &decl) {
  return decl.objc_foreign_declared || decl.objc_import_module_declared ||
         decl.objc_export_header_declared || decl.objc_mixed_image_declared ||
         decl.objc_package_entry_declared;
}

template <typename CallableDeclT>
bool HasInteropCppInteropAnnotations(const CallableDeclT &decl) {
  return decl.objc_cxx_name_declared || decl.objc_header_name_declared ||
         decl.objc_abi_align_declared || decl.objc_foreign_type_declared;
}

template <typename CallableDeclT>
bool HasInteropSwiftInteropAnnotations(const CallableDeclT &decl) {
  return decl.objc_swift_name_declared || decl.objc_swift_private_declared;
}

template <typename CallableDeclT>
bool HasInteropOwnershipInteractionSurface(const CallableDeclT &decl) {
  if (decl.has_return_ownership_qualifier ||
      decl.objc_returns_borrowed_declared ||
      decl.return_borrowed_pointer_qualified ||
      !decl.retainable_c_family_callable_attributes.empty()) {
    return true;
  }
  return std::any_of(
      decl.params.begin(), decl.params.end(),
      [](const FuncParam &param) { return param.has_ownership_qualifier; });
}

template <typename CallableDeclT>
bool HasInteropAsyncInteractionSurface(const CallableDeclT &decl) {
  return decl.async_declared || decl.executor_affinity_declared;
}
