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
      << ";typed_throws="
      << (function.typed_throws_declared ? "true" : "false")
      << ";typed_throws_payload="
      << function.typed_throws_payload.canonical_spelling
      << ";return_value_optional="
      << (function.return_value_optional.present ? "true" : "false")
      << ";return_value_optional_payload="
      << function.return_value_optional.payload_type_spelling
      << ";return_value_optional_abi="
      << function.return_value_optional.abi_layout_id
      << ";pure=" << (function.is_pure ? "true" : "false")
      << ";body=" << (!function.body.empty() ? "true" : "false")
      << ";owner=" << function.scope_owner_symbol;
  return out.str();
}
