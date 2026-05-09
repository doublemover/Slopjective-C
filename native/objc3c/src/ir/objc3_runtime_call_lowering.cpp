#include "ir/objc3_runtime_call_lowering.h"

void Objc3RuntimeCallLoweringState::Reset() {
  runtime_dispatch_symbols_used.clear();
  runtime_dispatch_call_emitted = false;
  direct_dispatch_call_sites_emitted = 0;
  runtime_dispatch_call_sites_emitted = 0;
  selector_pool_gep_sites_emitted = 0;
}

void Objc3RuntimeCallLoweringState::NoteDirectDispatchCall() {
  ++direct_dispatch_call_sites_emitted;
}

void Objc3RuntimeCallLoweringState::NoteSelectorPoolGep() {
  ++selector_pool_gep_sites_emitted;
}

void Objc3RuntimeCallLoweringState::NoteRuntimeDispatchCall(
    const std::string &dispatch_symbol) {
  runtime_dispatch_call_emitted = true;
  ++runtime_dispatch_call_sites_emitted;
  runtime_dispatch_symbols_used.insert(dispatch_symbol);
}

std::string BuildObjc3RuntimeDispatchCallIR(
    const Objc3RuntimeDispatchCallRequest &request) {
  std::ostringstream call;
  call << "  " << request.result_value << " = call i32 @"
       << request.dispatch_symbol << "(i32 " << request.receiver << ", ptr "
       << request.selector_ptr;
  for (const std::string &arg : request.args) {
    call << ", i32 " << arg;
  }
  call << ")";
  return call.str();
}

std::string BuildObjc3DirectDispatchCallIR(
    const Objc3DirectDispatchCallRequest &request) {
  std::ostringstream call;
  call << "  " << request.result_value << " = call i32 "
       << request.callee_symbol << "(";
  for (std::size_t i = 0;
       i < request.explicit_arg_count && i < request.args.size(); ++i) {
    if (i != 0) {
      call << ", ";
    }
    call << "i32 " << request.args[i];
  }
  call << ")";
  return call.str();
}

void EmitObjc3RuntimeDispatchDeclarations(
    const Objc3LoweringIRBoundary &boundary,
    const Objc3RuntimeCallLoweringState &state, std::ostringstream &out) {
  if (state.runtime_dispatch_symbols_used.empty()) {
    return;
  }
  for (const std::string &symbol : state.runtime_dispatch_symbols_used) {
    if (symbol.empty()) {
      continue;
    }
    out << "declare i32 @" << symbol << "(i32, ptr";
    for (std::size_t i = 0; i < boundary.runtime_dispatch_arg_slots; ++i) {
      out << ", i32";
    }
    out << ")\n";
  }
  out << "\n";
}
