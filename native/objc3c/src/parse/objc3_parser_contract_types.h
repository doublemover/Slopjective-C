#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "ast/objc3_ast_declarations.h"
#include "contracts/objc3_diagnostic_owner_contract.h"

// Parser-to-sema contract types. Keep parser outputs wrapped so downstream
// lowering/IR/runtime consumers bind to explicit parser contracts.
struct Objc3ParsedProgram {
  Objc3Program ast;
};

struct Objc3ParserContractSnapshot {
  std::size_t token_count = 0;
  std::size_t top_level_declaration_count = 0;
  std::size_t global_decl_count = 0;
  std::size_t protocol_decl_count = 0;
  std::size_t protocol_property_decl_count = 0;
  std::size_t protocol_method_decl_count = 0;
  std::size_t protocol_class_method_decl_count = 0;
  std::size_t protocol_instance_method_decl_count = 0;
  std::size_t interface_decl_count = 0;
  std::size_t interface_property_decl_count = 0;
  std::size_t interface_method_decl_count = 0;
  std::size_t interface_class_method_decl_count = 0;
  std::size_t interface_instance_method_decl_count = 0;
  std::size_t implementation_decl_count = 0;
  std::size_t implementation_property_decl_count = 0;
  std::size_t implementation_method_decl_count = 0;
  std::size_t implementation_class_method_decl_count = 0;
  std::size_t implementation_instance_method_decl_count = 0;
  std::size_t function_decl_count = 0;
  std::size_t interface_category_decl_count = 0;
  std::size_t implementation_category_decl_count = 0;
  std::size_t function_prototype_count = 0;
  std::size_t function_pure_count = 0;
  std::size_t draft_syntax_surface_count = 0;
  std::uint64_t draft_syntax_surface_fingerprint = 1469598103934665603ull;
  std::string draft_syntax_surface_handoff_key;
  bool draft_syntax_surface_handoff_deterministic = true;
  std::size_t long_tail_grammar_construct_count = 0;
  std::size_t long_tail_grammar_covered_construct_count = 0;
  std::uint64_t long_tail_grammar_fingerprint = 1469598103934665603ull;
  std::string long_tail_grammar_handoff_key;
  bool long_tail_grammar_handoff_deterministic = true;
  std::size_t parser_diagnostic_count = 0;
  std::string diagnostic_owner_contract_id = std::string(kObjc3DiagnosticOwnerContractId);
  std::string diagnostic_stage_owner = std::string(kObjc3ParserDiagnosticStageOwner);
  std::string diagnostic_fixit_owner = std::string(kObjc3ParserDiagnosticFixitOwner);
  std::string diagnostic_recovery_owner = std::string(kObjc3ParserDiagnosticRecoveryOwner);
  bool diagnostic_owner_split_explicit = true;
  bool diagnostic_recovery_counts_as_success = false;
  std::uint64_t ast_shape_fingerprint = 0;
  std::uint64_t ast_top_level_layout_fingerprint = 0;
  bool deterministic_handoff = true;
  bool parser_recovery_replay_ready = true;
};

using Objc3ParsedGlobalDecl = GlobalDecl;
using Objc3ParsedProtocolDecl = Objc3ProtocolDecl;
using Objc3ParsedInterfaceDecl = Objc3InterfaceDecl;
using Objc3ParsedImplementationDecl = Objc3ImplementationDecl;
using Objc3ParsedFunctionDecl = FunctionDecl;

inline Objc3Program &MutableObjc3ParsedProgramAst(Objc3ParsedProgram &program) {
  return program.ast;
}

inline const Objc3Program &Objc3ParsedProgramAst(
    const Objc3ParsedProgram &program) {
  return program.ast;
}
