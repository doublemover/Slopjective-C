#include "ir/objc3_ir_prototype_declarations.h"

#include <sstream>
#include <string>
#include <unordered_set>

#include "ast/objc3_ast_contracts.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_prototype_declarations_runtime_helpers.h"
#include "ir/objc3_ir_type_model.h"

namespace {

void EmitObjc3IRExternalFunctionDeclarations(
    const Objc3IRPrototypeDeclarationOptions &options,
    std::unordered_set<std::string> &declared_symbols, bool &emitted,
    std::ostringstream &out) {
  for (const auto &entry : options.function_signatures) {
    if (options.defined_functions.find(entry.first) !=
        options.defined_functions.end()) {
      continue;
    }
    if (!declared_symbols.insert(entry.first).second) {
      continue;
    }
    const LoweredFunctionSignature &signature = entry.second;
    std::ostringstream params;
    for (std::size_t i = 0; i < signature.param_types.size(); ++i) {
      if (i != 0) {
        params << ", ";
      }
      params << LLVMScalarType(signature.param_types[i]);
    }
    if (signature.throws_declared) {
      if (!signature.param_types.empty()) {
        params << ", ";
      }
      params << "ptr";
    }
    out << "declare " << LLVMScalarType(signature.return_type) << " @"
        << entry.first << "(" << params.str() << ")\n";
    emitted = true;
  }
}

}  // namespace

void EmitObjc3IRPrototypeDeclarations(
    const Objc3IRPrototypeDeclarationOptions &options, std::ostringstream &out) {
  bool emitted = false;
  std::unordered_set<std::string> declared_symbols;
  EmitObjc3IRDeclarationOnce(declared_symbols, emitted, out, "abort",
                             "declare void @abort()\n");
  if (options.emit_runtime_bootstrap_lowering) {
    EmitObjc3IRDeclarationOnce(
        declared_symbols, emitted, out,
        kObjc3RuntimeBootstrapStageRegistrationTableSymbol,
        "declare void @" +
            std::string(kObjc3RuntimeBootstrapStageRegistrationTableSymbol) +
            "(ptr)\n");
    EmitObjc3IRDeclarationOnce(
        declared_symbols, emitted, out,
        options.frontend_metadata
            .runtime_bootstrap_lowering_registration_entrypoint_symbol,
        "declare i32 @" +
            options.frontend_metadata
                .runtime_bootstrap_lowering_registration_entrypoint_symbol +
            "(ptr)\n");
  }
  if (Objc3IRRequiresRuntimeHelperDeclarations(options)) {
    EmitObjc3IRRuntimeHelperDeclarations(declared_symbols, emitted, out);
  }
  EmitObjc3IRExternalFunctionDeclarations(options, declared_symbols, emitted,
                                          out);
  if (emitted) {
    out << "\n";
  }
}
