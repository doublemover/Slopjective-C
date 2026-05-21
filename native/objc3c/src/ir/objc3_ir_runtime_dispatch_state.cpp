#include "ir/objc3_ir_runtime_dispatch_state.h"

#include "lower/contracts/runtime_dispatch_strict_abi_entrypoint_contracts.h"

void Objc3IRRuntimeDispatchCallState::Reset() {
  runtime_dispatch_symbols_used.clear();
  runtime_dispatch_call_emitted = false;
  direct_dispatch_call_sites_emitted = 0;
  runtime_dispatch_call_sites_emitted = 0;
  cache_aware_dispatch_call_sites_emitted = 0;
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

void Objc3IRRuntimeDispatchCallState::NoteCacheAwareDispatchCall() {
  runtime_dispatch_call_emitted = true;
  ++cache_aware_dispatch_call_sites_emitted;
  runtime_dispatch_symbols_used.insert(
      kObjc3RuntimePrepareCacheAwareDispatchDescriptorSymbol);
  runtime_dispatch_symbols_used.insert(
      kObjc3RuntimeCacheAwareDispatchI32CheckedSymbol);
}
