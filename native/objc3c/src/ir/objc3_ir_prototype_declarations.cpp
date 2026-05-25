#include "ir/objc3_ir_prototype_declarations.h"

#include <sstream>
#include <string>
#include <unordered_set>

#include "ast/objc3_ast_contracts.h"
#include "ast/objc3_ast_value_optional_type.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_prototype_declarations_runtime_helpers.h"
#include "ir/objc3_ir_type_model.h"
#include "support/objc3_identifier_spelling.h"

namespace {

bool EmitObjc3IRFullI64RuntimeHelperExternalDeclaration(
    const std::string &symbol, std::ostringstream &out) {
  if (symbol == kObjc3RuntimeOptionalAbsentFullI64Symbol) {
#if defined(_WIN32)
    out << "declare void @" << kObjc3RuntimeOptionalAbsentFullI64Symbol
        << "(ptr sret({ i8, i64 }) align 8)\n";
#else
    out << "declare { i8, i64 } @"
        << kObjc3RuntimeOptionalAbsentFullI64Symbol << "()\n";
#endif
    return true;
  }
  if (symbol == kObjc3RuntimeOptionalPresentFullI64Symbol) {
#if defined(_WIN32)
    out << "declare void @" << kObjc3RuntimeOptionalPresentFullI64Symbol
        << "(ptr sret({ i8, i64 }) align 8, i64)\n";
#else
    out << "declare { i8, i64 } @"
        << kObjc3RuntimeOptionalPresentFullI64Symbol << "(i64)\n";
#endif
    return true;
  }
  if (symbol == kObjc3RuntimeOptionalHasValueFullI64Symbol) {
    out << "declare i1 @" << kObjc3RuntimeOptionalHasValueFullI64Symbol
#if defined(_WIN32)
        << "(ptr)\n";
#else
        << "({ i8, i64 })\n";
#endif
    return true;
  }
  if (symbol == kObjc3RuntimeOptionalPayloadOrFullI64Symbol) {
    out << "declare i64 @" << kObjc3RuntimeOptionalPayloadOrFullI64Symbol
#if defined(_WIN32)
        << "(ptr, i64)\n";
#else
        << "({ i8, i64 }, i64)\n";
#endif
    return true;
  }
  if (symbol == kObjc3RuntimeOptionalUnwrapFullI64Symbol) {
    out << "declare i64 @" << kObjc3RuntimeOptionalUnwrapFullI64Symbol
#if defined(_WIN32)
        << "(ptr)\n";
#else
        << "({ i8, i64 })\n";
#endif
    return true;
  }
  return false;
}

void EmitObjc3IRExternalFunctionDeclarations(
    const Objc3IRPrototypeDeclarationOptions &options,
    std::unordered_set<std::string> &declared_symbols, bool &emitted,
    std::ostringstream &out) {
  for (const auto &entry : options.function_signatures) {
    if (!objc3c::support::IsObjcIdentifierSpelling(entry.first)) {
      continue;
    }
    if (options.defined_functions.find(entry.first) !=
        options.defined_functions.end()) {
      continue;
    }
    if (!declared_symbols.insert(entry.first).second) {
      continue;
    }
    if (EmitObjc3IRFullI64RuntimeHelperExternalDeclaration(entry.first, out)) {
      emitted = true;
      continue;
    }
    const LoweredFunctionSignature &signature = entry.second;
    if (signature.has_value_optional_type_signature &&
        !signature.value_optional_lowering_supported) {
      out << "; value optional semantic signature not emitted for @"
          << entry.first << " (O3P159)\n";
      emitted = true;
      continue;
    }
    std::ostringstream params;
    for (std::size_t i = 0; i < signature.param_types.size(); ++i) {
      if (i != 0) {
        params << ", ";
      }
      const Objc3IRValueOptionalCarrierMetadata *param_carrier =
          i < signature.param_value_optional_carriers.size()
              ? &signature.param_value_optional_carriers[i]
              : nullptr;
      Objc3IRValueOptionalCarrierMetadata default_carrier;
      params << LLVMScalarTypeForValueOptionalCarrier(
          signature.param_types[i],
          param_carrier != nullptr ? *param_carrier : default_carrier);
    }
    if (signature.throws_error_out_abi_ready) {
      if (!signature.param_types.empty()) {
        params << ", ";
      }
      params << "ptr";
    }
    out << "declare "
        << LLVMScalarTypeForValueOptionalCarrier(
               signature.return_type, signature.return_value_optional_carrier)
        << " @"
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
