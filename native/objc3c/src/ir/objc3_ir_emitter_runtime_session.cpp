#include "ir/objc3_ir_emitter_runtime_session.h"

Objc3IREmitterRuntimeSession::Objc3IREmitterRuntimeSession(
    const Objc3IREmitterRuntimeSessionInputs &inputs)
    : inputs_(inputs) {
  ResetForEmission();
}

void Objc3IREmitterRuntimeSession::ResetForEmission() {
  runtime_dispatch_call_state_.Reset();
  synthetic_method_stats_ = {};
  unsupported_fail_closed_path_triggered_ = false;
  unsupported_fail_closed_path_reason_.clear();
  block_function_definitions_.clear();
  emitted_block_invoke_symbols_.clear();
  emitted_block_copy_helper_symbols_.clear();
  emitted_block_dispose_helper_symbols_.clear();
}

Objc3IREmitterServiceContextState
Objc3IREmitterRuntimeSession::ServiceContextState() {
  const Objc3IREmitterStateInitialization &initialized_state =
      inputs_.initialized_state;
  return Objc3IREmitterServiceContextState{
      inputs_.program,
      inputs_.frontend_metadata,
      initialized_state.lowering_ir_boundary,
      initialized_state.runtime_metadata_symbols,
      initialized_state.globals,
      initialized_state.mutable_global_symbols,
      inputs_.global_const_values,
      inputs_.global_nil_proven_symbols,
      initialized_state.defined_functions,
      initialized_state.declared_pure_functions,
      initialized_state.function_definitions,
      initialized_state.method_definitions,
      initialized_state.metaprogramming_global_artifacts,
      initialized_state.impure_functions,
      initialized_state.function_arity,
      initialized_state.function_signatures,
      initialized_state.direct_dispatch_symbols_by_key,
      initialized_state.selector_pool_globals,
      initialized_state.runtime_string_pool_globals,
      initialized_state.typed_keypath_artifacts,
      initialized_state.class_receiver_constants,
      initialized_state.synthesized_property_accessor_count,
      initialized_state.vector_signature_function_count,
      block_function_definitions_,
      emitted_block_invoke_symbols_,
      emitted_block_copy_helper_symbols_,
      emitted_block_dispose_helper_symbols_,
      runtime_dispatch_call_state_,
      synthetic_method_stats_};
}

Objc3IREmitterServiceContextCallbacks
Objc3IREmitterRuntimeSession::ServiceContextCallbacks() {
  return BuildObjc3IREmitterServiceContextCallbacks(
      [this](const std::string &reason) {
        return EmitUnsupportedI32Value(reason);
      });
}

bool Objc3IREmitterRuntimeSession::UnsupportedFailClosedPathTriggered()
    const {
  return unsupported_fail_closed_path_triggered_;
}

std::string Objc3IREmitterRuntimeSession::UnsupportedFailClosedError() const {
  return "lowering encountered unsupported fail-closed path: " +
         unsupported_fail_closed_path_reason_;
}

const Objc3IRSyntheticMethodEmissionStats &
Objc3IREmitterRuntimeSession::SyntheticMethodStats() const {
  return synthetic_method_stats_;
}

std::string Objc3IREmitterRuntimeSession::EmitUnsupportedI32Value(
    const std::string &reason) {
  if (!unsupported_fail_closed_path_triggered_) {
    unsupported_fail_closed_path_triggered_ = true;
    unsupported_fail_closed_path_reason_ = reason;
  }
  return "poison";
}
