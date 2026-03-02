#pragma once

#include <cstddef>
#include <cstdint>

#include "sema/objc3_sema_pass_manager_contract.h"

inline Objc3ParserContractSnapshot ResolveObjc3ParserContractSnapshotForSemaHandoff(
    const Objc3SemaPassManagerInput &input) {
  if (input.parser_contract_snapshot != nullptr) {
    return *input.parser_contract_snapshot;
  }
  if (input.program == nullptr) {
    return Objc3ParserContractSnapshot{};
  }
  return BuildObjc3ParserContractSnapshot(*input.program, 0u, 0u);
}

inline bool IsObjc3ParserContractSnapshotConsistentWithProgram(const Objc3ParserContractSnapshot &snapshot,
                                                               const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  const std::uint64_t ast_shape_fingerprint = BuildObjc3ParsedProgramAstShapeFingerprint(program);
  const std::uint64_t ast_top_level_layout_fingerprint = BuildObjc3ParsedProgramTopLevelLayoutFingerprint(program);
  const Objc3ParserContractSnapshot expected_snapshot =
      BuildObjc3ParserContractSnapshot(program, snapshot.parser_diagnostic_count, snapshot.token_count);
  const std::size_t top_level_count =
      snapshot.global_decl_count + snapshot.protocol_decl_count + snapshot.interface_decl_count +
      snapshot.implementation_decl_count + snapshot.function_decl_count;
  return snapshot.global_decl_count == ast.globals.size() &&
         snapshot.protocol_decl_count == ast.protocols.size() &&
         snapshot.interface_decl_count == ast.interfaces.size() &&
         snapshot.implementation_decl_count == ast.implementations.size() &&
         snapshot.function_decl_count == ast.functions.size() &&
         snapshot.top_level_declaration_count == top_level_count &&
         snapshot.top_level_declaration_count ==
             ast.globals.size() + ast.protocols.size() + ast.interfaces.size() + ast.implementations.size() +
                 ast.functions.size() &&
         snapshot.ast_shape_fingerprint == ast_shape_fingerprint &&
         snapshot.ast_top_level_layout_fingerprint == ast_top_level_layout_fingerprint &&
         BuildObjc3ParserContractSnapshotFingerprint(snapshot) ==
             BuildObjc3ParserContractSnapshotFingerprint(expected_snapshot) &&
         snapshot.deterministic_handoff &&
         snapshot.parser_recovery_replay_ready;
}

struct Objc3ParserSemaHandoffScaffold {
  const Objc3ParsedProgram *program = nullptr;
  Objc3SemanticValidationOptions validation_options;
  Objc3SemaCompatibilityMode compatibility_mode = Objc3SemaCompatibilityMode::Canonical;
  bool migration_assist = false;
  Objc3SemaMigrationHints migration_hints;
  Objc3SemaDiagnosticsBus diagnostics_bus;
  Objc3ParserContractSnapshot parser_contract_snapshot;
  std::uint64_t expected_ast_shape_fingerprint = 0;
  bool parser_contract_ast_shape_fingerprint_matches = false;
  std::uint64_t expected_ast_top_level_layout_fingerprint = 0;
  bool parser_contract_ast_top_level_layout_fingerprint_matches = false;
  std::uint64_t expected_parser_contract_snapshot_fingerprint = 0;
  std::uint64_t parser_contract_snapshot_fingerprint = 0;
  bool parser_contract_snapshot_fingerprint_matches = false;
  bool parser_contract_snapshot_matches_program = false;
  bool deterministic = false;
};

inline Objc3ParserSemaHandoffScaffold BuildObjc3ParserSemaHandoffScaffold(const Objc3SemaPassManagerInput &input) {
  Objc3ParserSemaHandoffScaffold scaffold;
  scaffold.program = input.program;
  scaffold.validation_options = input.validation_options;
  scaffold.compatibility_mode = input.compatibility_mode;
  scaffold.migration_assist = input.migration_assist;
  scaffold.migration_hints = input.migration_hints;
  scaffold.diagnostics_bus = input.diagnostics_bus;
  if (input.program == nullptr) {
    return scaffold;
  }

  scaffold.parser_contract_snapshot = ResolveObjc3ParserContractSnapshotForSemaHandoff(input);
  scaffold.expected_ast_shape_fingerprint = BuildObjc3ParsedProgramAstShapeFingerprint(*input.program);
  scaffold.parser_contract_ast_shape_fingerprint_matches =
      scaffold.parser_contract_snapshot.ast_shape_fingerprint == scaffold.expected_ast_shape_fingerprint;
  scaffold.expected_ast_top_level_layout_fingerprint = BuildObjc3ParsedProgramTopLevelLayoutFingerprint(*input.program);
  scaffold.parser_contract_ast_top_level_layout_fingerprint_matches =
      scaffold.parser_contract_snapshot.ast_top_level_layout_fingerprint ==
      scaffold.expected_ast_top_level_layout_fingerprint;
  const Objc3ParserContractSnapshot expected_snapshot = BuildObjc3ParserContractSnapshot(
      *input.program,
      scaffold.parser_contract_snapshot.parser_diagnostic_count,
      scaffold.parser_contract_snapshot.token_count);
  scaffold.parser_contract_snapshot_fingerprint =
      BuildObjc3ParserContractSnapshotFingerprint(scaffold.parser_contract_snapshot);
  scaffold.expected_parser_contract_snapshot_fingerprint =
      BuildObjc3ParserContractSnapshotFingerprint(expected_snapshot);
  scaffold.parser_contract_snapshot_fingerprint_matches =
      scaffold.parser_contract_snapshot_fingerprint == scaffold.expected_parser_contract_snapshot_fingerprint;
  scaffold.parser_contract_snapshot_matches_program =
      IsObjc3ParserContractSnapshotConsistentWithProgram(scaffold.parser_contract_snapshot, *input.program);
  scaffold.deterministic = scaffold.parser_contract_snapshot_matches_program &&
                           scaffold.parser_contract_ast_shape_fingerprint_matches &&
                           scaffold.parser_contract_ast_top_level_layout_fingerprint_matches &&
                           scaffold.parser_contract_snapshot_fingerprint_matches;
  return scaffold;
}
