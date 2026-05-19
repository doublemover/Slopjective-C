#include "ast/objc3_ast_method_signature_surface.h"

#include <sstream>

#include "ast/objc3_ast_type_surface.h"

bool Objc3MethodDeclHasRuntimeBody(const Objc3MethodDecl &method) {
  return method.has_body && !method.body.empty();
}

bool Objc3MethodDeclRequiresRuntimeDispatch(const Objc3MethodDecl &method) {
  if (method.objc_direct_declared) {
    return false;
  }
  return method.is_class_method || method.has_body ||
         method.objc_dynamic_declared || method.objc_final_declared;
}

std::string Objc3MethodSignatureReplayKey(const Objc3MethodDecl &method) {
  std::ostringstream out;
  out << Objc3CallableSignatureReplayKey(method.selector, method.params,
                                         method.return_type,
                                         method.async_declared,
                                         method.throws_declared)
      << ";class_method=" << (method.is_class_method ? "true" : "false")
      << ";selector_normalized="
      << (method.selector_is_normalized ? "true" : "false")
      << ";selector_piece_count=" << method.selector_pieces.size()
      << ";direct=" << (method.objc_direct_declared ? "true" : "false")
      << ";dynamic=" << (method.objc_dynamic_declared ? "true" : "false")
      << ";final=" << (method.objc_final_declared ? "true" : "false")
      << ";body=" << (Objc3MethodDeclHasRuntimeBody(method) ? "true" : "false")
      << ";lookup=" << method.method_lookup_symbol
      << ";owner=" << method.scope_owner_symbol;
  return out.str();
}
