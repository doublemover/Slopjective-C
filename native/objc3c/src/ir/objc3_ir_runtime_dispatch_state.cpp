#include "ir/objc3_ir_runtime_dispatch_state.h"

void Objc3IRRuntimeDispatchCallState::Reset() {
  runtime_dispatch_symbols_used.clear();
  runtime_dispatch_call_emitted = false;
  direct_dispatch_call_sites_emitted = 0;
  runtime_dispatch_call_sites_emitted = 0;
  selector_pool_gep_sites_emitted = 0;
}

void Objc3IRRuntimeDispatchCallState::NoteDirectDispatchCall() {
  ++direct_dispatch_call_sites_emitted;
}

void Objc3IRRuntimeDispatchCallState::NoteSelectorPoolGep() {
  ++selector_pool_gep_sites_emitted;
}

void Objc3IRRuntimeDispatchCallState::NoteRuntimeDispatchCall(
    const std::string &dispatch_symbol) {
  runtime_dispatch_call_emitted = true;
  ++runtime_dispatch_call_sites_emitted;
  runtime_dispatch_symbols_used.insert(dispatch_symbol);
}
