#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline std::size_t Objc3ParserSnapshotDeclarationBreakdownCount(const Objc3ParserContractSnapshot &snapshot) {
  return snapshot.global_decl_count + snapshot.protocol_decl_count +
         snapshot.interface_decl_count + snapshot.implementation_decl_count +
         snapshot.function_decl_count;
}

inline std::size_t Objc3ParsedProgramTopLevelDeclarationCount(const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  return ast.globals.size() + ast.protocols.size() + ast.interfaces.size() +
         ast.implementations.size() + ast.functions.size();
}

inline std::string BuildObjc3ParseArtifactHandoffKey(
    const Objc3ParserContractSnapshot &snapshot,
    std::size_t ast_top_level_declaration_count,
    std::size_t parser_diagnostic_count,
    bool handoff_deterministic) {
  return "parser_snapshot=" + std::to_string(snapshot.top_level_declaration_count) + ":" +
         std::to_string(snapshot.global_decl_count) + ":" +
         std::to_string(snapshot.protocol_decl_count) + ":" +
         std::to_string(snapshot.interface_decl_count) + ":" +
         std::to_string(snapshot.implementation_decl_count) + ":" +
         std::to_string(snapshot.function_decl_count) + ";ast_top_level=" +
         std::to_string(ast_top_level_declaration_count) + ";parser_diagnostics=" +
         std::to_string(parser_diagnostic_count) + ";deterministic=" +
         (handoff_deterministic ? "true" : "false");
}

inline std::string BuildObjc3ParseArtifactReplayKey(
    const Objc3ParserContractSnapshot &snapshot,
    std::uint64_t parser_contract_snapshot_fingerprint,
    std::uint64_t parser_ast_top_level_layout_fingerprint,
    std::uint64_t ast_shape_fingerprint,
    std::uint64_t ast_top_level_layout_fingerprint,
    const std::string &compatibility_handoff_key,
    bool fingerprint_consistent,
    bool replay_key_deterministic) {
  return "parser_snapshot_fingerprint=" + std::to_string(parser_contract_snapshot_fingerprint) +
         ";snapshot_ast_shape_fingerprint=" + std::to_string(snapshot.ast_shape_fingerprint) +
         ";ast_shape_fingerprint=" + std::to_string(ast_shape_fingerprint) +
         ";snapshot_ast_top_level_layout_fingerprint=" +
         std::to_string(parser_ast_top_level_layout_fingerprint) +
         ";ast_top_level_layout_fingerprint=" + std::to_string(ast_top_level_layout_fingerprint) +
         ";compatibility_handoff_key=" + compatibility_handoff_key +
         ";fingerprint_consistent=" + (fingerprint_consistent ? "true" : "false") +
         ";deterministic=" + (replay_key_deterministic ? "true" : "false");
}

inline const char *Objc3FrontendLanguageProfileName(const Objc3FrontendLanguageProfile mode) {
  (void)mode;
  return "canonical";
}

inline bool IsObjc3LanguageVersionPragmaContractConsistent(
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract) {
  if (!pragma_contract.seen) {
    return pragma_contract.directive_count == 0 &&
           !pragma_contract.duplicate &&
           !pragma_contract.non_leading &&
           pragma_contract.first_line == 0 &&
           pragma_contract.first_column == 0 &&
           pragma_contract.last_line == 0 &&
           pragma_contract.last_column == 0;
  }

  const bool coordinates_present =
      pragma_contract.first_line > 0 &&
      pragma_contract.first_column > 0 &&
      pragma_contract.last_line > 0 &&
      pragma_contract.last_column > 0;
  const bool duplicate_consistent =
      !pragma_contract.duplicate || pragma_contract.directive_count > 1;
  return pragma_contract.directive_count > 0 &&
         coordinates_present &&
         duplicate_consistent;
}

inline bool IsObjc3LanguageVersionPragmaCoordinateOrderConsistent(
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract) {
  if (!pragma_contract.seen) {
    return true;
  }

  const bool first_before_or_equal_last =
      pragma_contract.first_line < pragma_contract.last_line ||
      (pragma_contract.first_line == pragma_contract.last_line &&
       pragma_contract.first_column <= pragma_contract.last_column);
  const bool single_directive_coordinates_consistent =
      pragma_contract.directive_count != 1 ||
      (pragma_contract.first_line == pragma_contract.last_line &&
       pragma_contract.first_column == pragma_contract.last_column);
  return first_before_or_equal_last &&
         single_directive_coordinates_consistent;
}

inline std::string BuildObjc3CompatibilityHandoffKey(
    const Objc3FrontendOptions &options,
    const Objc3FrontendCanonicalLiteralRejectionCounts
        &canonical_literal_rejection_counts,
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract,
    bool compatibility_handoff_consistent) {
  return "language_profile=" +
         std::string(Objc3FrontendLanguageProfileName(options.language_profile)) +
         ";canonical_literal_rejections=" +
         std::to_string(canonical_literal_rejection_counts.yes_literal_sites) +
         ":" +
         std::to_string(canonical_literal_rejection_counts.no_literal_sites) +
         ":" +
         std::to_string(canonical_literal_rejection_counts.null_literal_sites) +
         ";language_version_pragma=" + (pragma_contract.seen ? "seen" : "none") + ":" +
         std::to_string(pragma_contract.directive_count) + ":" +
         (pragma_contract.duplicate ? "duplicate" : "single") + ":" +
         (pragma_contract.non_leading ? "non-leading" : "leading") +
         ";consistent=" + (compatibility_handoff_consistent ? "true" : "false");
}
