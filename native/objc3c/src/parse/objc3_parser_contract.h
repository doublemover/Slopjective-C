#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "parse/objc3_parser_contract_fingerprints.h"
#include "parse/objc3_parser_contract_types.h"

inline std::uint64_t BuildObjc3ParsedProgramAstShapeFingerprint(const Objc3ParsedProgram &program) {
  constexpr std::uint64_t kInitialFingerprint = 1469598103934665603ull;
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::uint64_t fingerprint = kInitialFingerprint;
  fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, ast.module_name);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(ast.globals.size()));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(ast.protocols.size()));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(ast.interfaces.size()));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(ast.implementations.size()));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(ast.functions.size()));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(ast.draft_syntax_surface_summary.draft_syntax_surface_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      ast.draft_syntax_surface_summary.normalized ? 1ull : 0ull);
  fingerprint = MixObjc3ParserContractFingerprintString(
      fingerprint,
      ast.draft_syntax_surface_summary.replay_key);

  for (const auto &global : ast.globals) {
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, global.name);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(global.line));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(global.column));
  }
  for (const auto &protocol_decl : ast.protocols) {
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, protocol_decl.name);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(protocol_decl.properties.size()));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(protocol_decl.methods.size()));
    fingerprint =
        MixObjc3ParserContractFingerprint(fingerprint, protocol_decl.is_forward_declaration ? 1ull : 0ull);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(protocol_decl.line));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(protocol_decl.column));
  }
  for (const auto &interface_decl : ast.interfaces) {
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, interface_decl.name);
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, interface_decl.super_name);
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, interface_decl.category_name);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, interface_decl.has_category ? 1ull : 0ull);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(interface_decl.properties.size()));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(interface_decl.methods.size()));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(interface_decl.line));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(interface_decl.column));
  }
  for (const auto &implementation_decl : ast.implementations) {
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, implementation_decl.name);
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, implementation_decl.category_name);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, implementation_decl.has_category ? 1ull : 0ull);
    fingerprint =
        MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(implementation_decl.properties.size()));
    fingerprint =
        MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(implementation_decl.methods.size()));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(implementation_decl.line));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(implementation_decl.column));
  }
  for (const auto &function_decl : ast.functions) {
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, function_decl.name);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(function_decl.params.size()));
    fingerprint = MixObjc3ParserContractFingerprint(
        fingerprint,
        static_cast<std::uint64_t>(static_cast<std::uint8_t>(function_decl.return_type)));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, function_decl.is_prototype ? 1ull : 0ull);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, function_decl.is_pure ? 1ull : 0ull);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(function_decl.line));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(function_decl.column));
  }
  return fingerprint;
}

inline std::uint64_t BuildObjc3ParsedProgramTopLevelLayoutFingerprint(const Objc3ParsedProgram &program) {
  constexpr std::uint64_t kInitialFingerprint = 1469598103934665603ull;

  struct Objc3TopLevelLayoutEntry {
    std::uint64_t kind_tag = 0;
    std::string symbol;
    unsigned line = 1;
    unsigned column = 1;
  };

  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::vector<Objc3TopLevelLayoutEntry> entries;
  entries.reserve(ast.globals.size() + ast.protocols.size() + ast.interfaces.size() + ast.implementations.size() +
                  ast.functions.size());

  for (const auto &global : ast.globals) {
    entries.push_back(Objc3TopLevelLayoutEntry{1ull, global.name, global.line, global.column});
  }
  for (const auto &protocol_decl : ast.protocols) {
    entries.push_back(Objc3TopLevelLayoutEntry{2ull, protocol_decl.name, protocol_decl.line, protocol_decl.column});
  }
  for (const auto &interface_decl : ast.interfaces) {
    entries.push_back(Objc3TopLevelLayoutEntry{3ull, interface_decl.name, interface_decl.line, interface_decl.column});
  }
  for (const auto &implementation_decl : ast.implementations) {
    entries.push_back(
        Objc3TopLevelLayoutEntry{4ull, implementation_decl.name, implementation_decl.line, implementation_decl.column});
  }
  for (const auto &function_decl : ast.functions) {
    entries.push_back(Objc3TopLevelLayoutEntry{5ull, function_decl.name, function_decl.line, function_decl.column});
  }

  std::sort(entries.begin(), entries.end(), [](const Objc3TopLevelLayoutEntry &lhs, const Objc3TopLevelLayoutEntry &rhs) {
    if (lhs.line != rhs.line) {
      return lhs.line < rhs.line;
    }
    if (lhs.column != rhs.column) {
      return lhs.column < rhs.column;
    }
    if (lhs.kind_tag != rhs.kind_tag) {
      return lhs.kind_tag < rhs.kind_tag;
    }
    return lhs.symbol < rhs.symbol;
  });

  std::uint64_t fingerprint = kInitialFingerprint;
  fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, ast.module_name);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(entries.size()));
  for (const auto &entry : entries) {
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, entry.kind_tag);
    fingerprint = MixObjc3ParserContractFingerprintString(fingerprint, entry.symbol);
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(entry.line));
    fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(entry.column));
  }
  return fingerprint;
}

inline std::size_t BuildObjc3LongTailGrammarConstructCount(const Objc3ParserContractSnapshot &snapshot) {
  return snapshot.protocol_property_decl_count + snapshot.protocol_method_decl_count +
         snapshot.interface_property_decl_count + snapshot.interface_method_decl_count +
         snapshot.implementation_property_decl_count + snapshot.implementation_method_decl_count +
         snapshot.interface_category_decl_count + snapshot.implementation_category_decl_count +
         snapshot.function_prototype_count + snapshot.function_pure_count;
}

inline std::size_t BuildObjc3LongTailGrammarCoveredConstructCount(const Objc3ParserContractSnapshot &snapshot) {
  return snapshot.protocol_class_method_decl_count + snapshot.protocol_instance_method_decl_count +
         snapshot.interface_class_method_decl_count + snapshot.interface_instance_method_decl_count +
         snapshot.implementation_class_method_decl_count + snapshot.implementation_instance_method_decl_count +
         snapshot.function_prototype_count + snapshot.function_pure_count;
}

inline std::uint64_t BuildObjc3LongTailGrammarFingerprint(const Objc3ParserContractSnapshot &snapshot) {
  constexpr std::uint64_t kInitialFingerprint = 1469598103934665603ull;
  std::uint64_t fingerprint = kInitialFingerprint;
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.long_tail_grammar_construct_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.long_tail_grammar_covered_construct_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.protocol_property_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.protocol_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.interface_property_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.interface_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_property_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.interface_category_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_category_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.function_prototype_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.function_pure_count));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.ast_shape_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.ast_top_level_layout_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.draft_syntax_surface_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.parser_diagnostic_count));
  return fingerprint;
}

inline std::uint64_t BuildObjc3DraftSyntaxSurfaceFingerprint(
    const Objc3DraftSyntaxSurfaceSummary &summary) {
  constexpr std::uint64_t kInitialFingerprint = 1469598103934665603ull;
  std::uint64_t fingerprint = kInitialFingerprint;
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.block_literal_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.block_explicit_capture_list_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.block_explicit_capture_byref_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.block_byref_capture_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.block_heap_escape_candidate_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.try_expression_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.throw_statement_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.do_catch_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.error_catch_clause_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.error_catch_binding_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.error_catch_all_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.error_bridge_payload_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(
          summary.error_foreign_boundary_annotation_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.error_nested_cleanup_marker_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.throws_callable_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.async_callable_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.await_expression_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.actor_interface_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.actor_nonisolated_callable_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.actor_isolation_marker_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.actor_sendable_annotation_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.task_runtime_construct_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.task_cancellation_check_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.task_suspension_point_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.macro_attribute_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.macro_package_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.macro_provenance_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.macro_cache_key_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.macro_sandbox_policy_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.property_behavior_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.property_attribute_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.property_accessor_selector_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.property_synthesis_metadata_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.property_reflection_input_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(
          summary.property_ownership_nullability_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_attribute_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_import_module_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_swift_annotation_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_cxx_annotation_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_header_import_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_header_export_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_abi_alignment_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_foreign_type_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_mixed_image_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_package_entry_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_error_bridge_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.draft_syntax_surface_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      summary.normalized ? 1ull : 0ull);
  fingerprint = MixObjc3ParserContractFingerprintString(
      fingerprint,
      summary.replay_key);
  return fingerprint;
}

inline std::string BuildObjc3LongTailGrammarHandoffKey(
    const Objc3ParserContractSnapshot &snapshot,
    const bool handoff_deterministic) {
  return "long-tail-grammar:v1:constructs=" +
         std::to_string(snapshot.long_tail_grammar_construct_count) + ";covered=" +
         std::to_string(snapshot.long_tail_grammar_covered_construct_count) + ";fingerprint=" +
         std::to_string(snapshot.long_tail_grammar_fingerprint) + ";ast_shape_fingerprint=" +
         std::to_string(snapshot.ast_shape_fingerprint) + ";ast_top_level_layout_fingerprint=" +
         std::to_string(snapshot.ast_top_level_layout_fingerprint) + ";parser_diagnostics=" +
         std::to_string(snapshot.parser_diagnostic_count) + ";deterministic=" +
         (handoff_deterministic ? "true" : "false");
}

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
