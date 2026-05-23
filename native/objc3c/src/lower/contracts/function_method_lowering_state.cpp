#include "lower/contracts/function_method_lowering_state.h"

#include "ast/objc3_ast_decl_surface.h"
#include "ast/objc3_ast_type_surface.h"

#include <sstream>

namespace {

bool ParamsAreArcSensitive(const std::vector<FuncParam> &params) {
  for (const FuncParam &param : params) {
    if (param.has_ownership_qualifier || param.ownership_insert_retain ||
        param.ownership_insert_release || param.ownership_insert_autorelease ||
        Objc3ValueTypeIsObjectReference(param.type)) {
      return true;
    }
  }
  return false;
}

}  // namespace

Objc3CallableLoweringState Objc3BuildFunctionLoweringState(
    const FunctionDecl &function) {
  Objc3CallableLoweringState state;
  state.owner_symbol = function.scope_owner_symbol;
  state.callable_name = function.name;
  state.signature_replay_key = Objc3FunctionSignatureReplayKey(function);
  state.has_body = !function.body.empty();
  state.async_declared = function.async_declared;
  state.throws_declared = function.throws_declared;
  state.typed_throws_declared = function.typed_throws_declared;
  state.throws_error_out_abi_ready =
      function.throws_declared && !function.typed_throws_declared;
  state.typed_throws_error_type_spelling =
      function.typed_throws_payload.canonical_spelling;
  state.parameter_count = function.params.size();
  state.arc_sensitive = ParamsAreArcSensitive(function.params) ||
                        Objc3ValueTypeIsObjectReference(function.return_type);
  state.replay_key = Objc3CallableLoweringStateReplayKey(state);
  return state;
}

Objc3CallableLoweringState Objc3BuildMethodLoweringState(
    const Objc3MethodDecl &method) {
  Objc3CallableLoweringState state;
  state.owner_symbol = method.scope_owner_symbol;
  state.callable_name = method.selector;
  state.signature_replay_key = Objc3MethodSignatureReplayKey(method);
  state.method = true;
  state.class_method = method.is_class_method;
  state.has_body = Objc3MethodDeclHasRuntimeBody(method);
  state.async_declared = method.async_declared;
  state.throws_declared = method.throws_declared;
  state.typed_throws_declared = method.typed_throws_declared;
  state.throws_error_out_abi_ready =
      method.throws_declared && !method.typed_throws_declared;
  state.typed_throws_error_type_spelling =
      method.typed_throws_payload.canonical_spelling;
  state.runtime_dispatch_required = Objc3MethodDeclRequiresRuntimeDispatch(method);
  state.parameter_count = method.params.size();
  state.arc_sensitive = ParamsAreArcSensitive(method.params) ||
                        Objc3ValueTypeIsObjectReference(method.return_type);
  state.replay_key = Objc3CallableLoweringStateReplayKey(state);
  return state;
}

bool Objc3CallableLoweringStateRequiresRuntimeHelpers(
    const Objc3CallableLoweringState &state) {
  return state.runtime_dispatch_required || state.arc_sensitive ||
         state.async_declared || state.throws_error_out_abi_ready;
}

std::string Objc3CallableLoweringStateReplayKey(
    const Objc3CallableLoweringState &state) {
  std::ostringstream out;
  out << "owner=" << state.owner_symbol << ";callable=" << state.callable_name
      << ";method=" << (state.method ? "true" : "false")
      << ";class_method=" << (state.class_method ? "true" : "false")
      << ";body=" << (state.has_body ? "true" : "false")
      << ";async=" << (state.async_declared ? "true" : "false")
      << ";throws=" << (state.throws_declared ? "true" : "false")
      << ";typed_throws="
      << (state.typed_throws_declared ? "true" : "false")
      << ";throws_error_out_abi_ready="
      << (state.throws_error_out_abi_ready ? "true" : "false")
      << ";typed_throws_payload=" << state.typed_throws_error_type_spelling
      << ";runtime_dispatch="
      << (state.runtime_dispatch_required ? "true" : "false")
      << ";arc_sensitive=" << (state.arc_sensitive ? "true" : "false")
      << ";params=" << state.parameter_count
      << ";signature={" << state.signature_replay_key << "}";
  return out.str();
}
