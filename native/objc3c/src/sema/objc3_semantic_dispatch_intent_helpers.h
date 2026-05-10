#pragma once

#include "ast/objc3_ast_declarations.h"

template <typename CallableDeclT>
bool HasDispatchCallableDispatchIntentAttributes(const CallableDeclT &decl) {
  return decl.objc_direct_declared || decl.objc_final_declared ||
         decl.objc_dynamic_declared;
}

inline bool HasDispatchContainerDispatchIntentAttributes(
    const Objc3InterfaceDecl &decl) {
  return decl.objc_direct_members_declared || decl.objc_final_declared ||
         decl.objc_sealed_declared;
}
