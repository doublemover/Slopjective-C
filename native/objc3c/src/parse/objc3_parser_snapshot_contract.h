#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>

#include "parse/objc3_parser_ast_fingerprints_contract.h"
#include "parse/objc3_parser_contract_fingerprints.h"
#include "parse/objc3_parser_contract_types.h"
#include "parse/objc3_parser_draft_syntax_surface_contract.h"
#include "parse/objc3_parser_long_tail_grammar_contract.h"

inline std::uint64_t BuildObjc3ParserContractSnapshotFingerprint(const Objc3ParserContractSnapshot &snapshot) {
  constexpr std::uint64_t kInitialFingerprint = 1469598103934665603ull;
  std::uint64_t fingerprint = kInitialFingerprint;
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.token_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.top_level_declaration_count));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.global_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.protocol_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.protocol_property_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.protocol_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.protocol_class_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.protocol_instance_method_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.interface_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.interface_property_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.interface_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.interface_class_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.interface_instance_method_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.implementation_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_property_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_class_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_instance_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.function_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.interface_category_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_category_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.function_prototype_count));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.function_pure_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.draft_syntax_surface_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      snapshot.draft_syntax_surface_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprintString(
      fingerprint,
      snapshot.draft_syntax_surface_handoff_key);
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      snapshot.draft_syntax_surface_handoff_deterministic ? 1ull : 0ull);
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.long_tail_grammar_construct_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.long_tail_grammar_covered_construct_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      snapshot.long_tail_grammar_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprintString(
      fingerprint,
      snapshot.long_tail_grammar_handoff_key);
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      snapshot.long_tail_grammar_handoff_deterministic ? 1ull : 0ull);
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.parser_diagnostic_count));
  fingerprint = MixObjc3ParserContractFingerprintString(
      fingerprint,
      snapshot.diagnostic_owner_contract_id);
  fingerprint = MixObjc3ParserContractFingerprintString(
      fingerprint,
      snapshot.diagnostic_stage_owner);
  fingerprint = MixObjc3ParserContractFingerprintString(
      fingerprint,
      snapshot.diagnostic_fixit_owner);
  fingerprint = MixObjc3ParserContractFingerprintString(
      fingerprint,
      snapshot.diagnostic_recovery_owner);
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      snapshot.diagnostic_owner_split_explicit ? 1ull : 0ull);
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      snapshot.diagnostic_recovery_counts_as_success ? 1ull : 0ull);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.ast_shape_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.ast_top_level_layout_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.deterministic_handoff ? 1ull : 0ull);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.parser_recovery_replay_ready ? 1ull : 0ull);
  return fingerprint;
}

inline Objc3ParserContractSnapshot BuildObjc3ParserContractSnapshot(
    const Objc3ParsedProgram &program,
    const std::size_t parser_diagnostic_count,
    const std::size_t token_count) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  Objc3ParserContractSnapshot snapshot;
  snapshot.token_count = token_count;
  snapshot.global_decl_count = ast.globals.size();
  snapshot.protocol_decl_count = ast.protocols.size();
  for (const auto &protocol_decl : ast.protocols) {
    snapshot.protocol_property_decl_count += protocol_decl.properties.size();
    snapshot.protocol_method_decl_count += protocol_decl.methods.size();
    for (const auto &method_decl : protocol_decl.methods) {
      if (method_decl.is_class_method) {
        ++snapshot.protocol_class_method_decl_count;
      } else {
        ++snapshot.protocol_instance_method_decl_count;
      }
    }
  }
  snapshot.interface_decl_count = ast.interfaces.size();
  for (const auto &interface_decl : ast.interfaces) {
    snapshot.interface_property_decl_count += interface_decl.properties.size();
    snapshot.interface_method_decl_count += interface_decl.methods.size();
    for (const auto &method_decl : interface_decl.methods) {
      if (method_decl.is_class_method) {
        ++snapshot.interface_class_method_decl_count;
      } else {
        ++snapshot.interface_instance_method_decl_count;
      }
    }
  }
  snapshot.implementation_decl_count = ast.implementations.size();
  for (const auto &implementation_decl : ast.implementations) {
    snapshot.implementation_property_decl_count += implementation_decl.properties.size();
    snapshot.implementation_method_decl_count += implementation_decl.methods.size();
    for (const auto &method_decl : implementation_decl.methods) {
      if (method_decl.is_class_method) {
        ++snapshot.implementation_class_method_decl_count;
      } else {
        ++snapshot.implementation_instance_method_decl_count;
      }
    }
  }
  snapshot.function_decl_count = ast.functions.size();
  snapshot.interface_category_decl_count = static_cast<std::size_t>(std::count_if(
      ast.interfaces.begin(),
      ast.interfaces.end(),
      [](const Objc3InterfaceDecl &interface_decl) { return interface_decl.has_category; }));
  snapshot.implementation_category_decl_count = static_cast<std::size_t>(std::count_if(
      ast.implementations.begin(),
      ast.implementations.end(),
      [](const Objc3ImplementationDecl &implementation_decl) { return implementation_decl.has_category; }));
  snapshot.function_prototype_count = static_cast<std::size_t>(std::count_if(
      ast.functions.begin(),
      ast.functions.end(),
      [](const FunctionDecl &function_decl) { return function_decl.is_prototype; }));
  snapshot.function_pure_count = static_cast<std::size_t>(std::count_if(
      ast.functions.begin(),
      ast.functions.end(),
      [](const FunctionDecl &function_decl) { return function_decl.is_pure; }));
  snapshot.draft_syntax_surface_count =
      ast.draft_syntax_surface_summary.draft_syntax_surface_sites;
  snapshot.draft_syntax_surface_fingerprint =
      BuildObjc3DraftSyntaxSurfaceFingerprint(ast.draft_syntax_surface_summary);
  snapshot.draft_syntax_surface_handoff_key =
      ast.draft_syntax_surface_summary.replay_key;
  snapshot.draft_syntax_surface_handoff_deterministic =
      ast.draft_syntax_surface_summary.normalized &&
      snapshot.draft_syntax_surface_fingerprint != 0u &&
      !snapshot.draft_syntax_surface_handoff_key.empty();
  snapshot.ast_shape_fingerprint = BuildObjc3ParsedProgramAstShapeFingerprint(program);
  snapshot.ast_top_level_layout_fingerprint = BuildObjc3ParsedProgramTopLevelLayoutFingerprint(program);
  snapshot.top_level_declaration_count = snapshot.global_decl_count + snapshot.protocol_decl_count +
                                         snapshot.interface_decl_count + snapshot.implementation_decl_count +
                                         snapshot.function_decl_count;
  snapshot.parser_diagnostic_count = parser_diagnostic_count;
  snapshot.diagnostic_owner_contract_id = std::string(kObjc3DiagnosticOwnerContractId);
  snapshot.diagnostic_stage_owner = std::string(kObjc3ParserDiagnosticStageOwner);
  snapshot.diagnostic_fixit_owner = std::string(kObjc3ParserDiagnosticFixitOwner);
  snapshot.diagnostic_recovery_owner = std::string(kObjc3ParserDiagnosticRecoveryOwner);
  snapshot.diagnostic_owner_split_explicit =
      Objc3DiagnosticStageIsHardCutover(Objc3FrontendDiagnosticStage::kParser) &&
      Objc3DiagnosticOwnerIsExplicit(snapshot.diagnostic_stage_owner) &&
      Objc3DiagnosticOwnerIsExplicit(snapshot.diagnostic_fixit_owner) &&
      Objc3DiagnosticOwnerIsExplicit(snapshot.diagnostic_recovery_owner);
  snapshot.diagnostic_recovery_counts_as_success = false;
  snapshot.deterministic_handoff =
      snapshot.diagnostic_owner_split_explicit &&
      !snapshot.diagnostic_recovery_counts_as_success;
  snapshot.parser_recovery_replay_ready = true;
  snapshot.long_tail_grammar_construct_count = BuildObjc3LongTailGrammarConstructCount(snapshot);
  snapshot.long_tail_grammar_covered_construct_count = BuildObjc3LongTailGrammarCoveredConstructCount(snapshot);
  snapshot.long_tail_grammar_fingerprint = BuildObjc3LongTailGrammarFingerprint(snapshot);
  const bool long_tail_grammar_counts_consistent =
      snapshot.long_tail_grammar_covered_construct_count <= snapshot.long_tail_grammar_construct_count;
  snapshot.long_tail_grammar_handoff_deterministic =
      snapshot.long_tail_grammar_fingerprint != 0 &&
      long_tail_grammar_counts_consistent &&
      snapshot.deterministic_handoff &&
      snapshot.parser_recovery_replay_ready;
  snapshot.long_tail_grammar_handoff_key = BuildObjc3LongTailGrammarHandoffKey(
      snapshot,
      snapshot.long_tail_grammar_handoff_deterministic);
  snapshot.long_tail_grammar_handoff_deterministic =
      snapshot.long_tail_grammar_handoff_deterministic &&
      !snapshot.long_tail_grammar_handoff_key.empty();
  return snapshot;
}

inline Objc3ParserContractSnapshot BuildObjc3ParserContractSnapshot(
    const Objc3ParsedProgram &program,
    const std::size_t parser_diagnostic_count) {
  return BuildObjc3ParserContractSnapshot(program, parser_diagnostic_count, 0u);
}
