#include "ir/objc3_ir_runtime_dispatch_declarations.h"

#include <cstddef>
#include <string>

#include "lower/contracts/runtime_dispatch_strict_abi_entrypoint_contracts.h"

void EmitObjc3IRRuntimeDispatchDeclarations(
    const Objc3LoweringIRBoundary &boundary,
    const Objc3IRRuntimeDispatchCallState &state, std::ostringstream &out) {
  if (state.runtime_dispatch_symbols_used.empty()) {
    return;
  }
  for (const std::string &symbol : state.runtime_dispatch_symbols_used) {
    if (symbol.empty()) {
      continue;
    }
    if (symbol == kObjc3RuntimeTypedDispatchValueFromClassSymbol) {
      out << "declare i32 @" << symbol << "(i32, i32, ptr, ptr";
    } else if (symbol == kObjc3RuntimeTypedDispatchValueSymbol) {
      out << "declare i32 @" << symbol << "(i32, i32, ptr";
    } else {
      out << "declare i32 @" << symbol << "(i32, ptr";
    }
    for (std::size_t i = 0; i < boundary.runtime_dispatch_arg_slots; ++i) {
      out << ", i32";
    }
    out << ")\n";
  }
  out << "\n";
}
