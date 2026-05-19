#include "ast/objc3_ast_function_signature_surface.h"

#include <sstream>

#include "ast/objc3_ast_type_surface.h"

std::string Objc3FunctionSignatureReplayKey(const FunctionDecl &function) {
  std::ostringstream out;
  out << Objc3CallableSignatureReplayKey(function.name, function.params,
                                         function.return_type,
                                         function.async_declared,
                                         function.throws_declared)
      << ";prototype=" << (function.is_prototype ? "true" : "false")
      << ";pure=" << (function.is_pure ? "true" : "false")
      << ";body=" << (!function.body.empty() ? "true" : "false")
      << ";owner=" << function.scope_owner_symbol;
  return out.str();
}
