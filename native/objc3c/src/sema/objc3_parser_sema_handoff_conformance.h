#pragma once

#include "sema/objc3_parser_sema_handoff_decl_counts.h"

inline constexpr std::size_t kObjc3ParserSemaConformanceMatrixBuilderMaxLines = 190u;
inline constexpr std::size_t kObjc3ParserSemaConformanceCorpusBuilderMaxLines = 75u;
inline constexpr std::size_t kObjc3ParserSemaHandoffScaffoldBuilderMaxLines = 80u;

inline Objc3ParserSemaConformanceMatrix BuildObjc3ParserSemaConformanceMatrix(
    const Objc3ParserContractSnapshot &snapshot, const Objc3ParsedProgram &program) {
  Objc3ParserSemaConformanceMatrix matrix;
  const Objc3ParserContractSnapshot expected_snapshot =
      BuildObjc3ParserContractSnapshot(program, snapshot.parser_diagnostic_count, snapshot.token_count);

  matrix.parser_top_level_declaration_count = snapshot.top_level_declaration_count;
  matrix.ast_top_level_declaration_count = expected_snapshot.top_level_declaration_count;
  matrix.parser_global_decl_count = snapshot.global_decl_count;
  matrix.ast_global_decl_count = expected_snapshot.global_decl_count;
  matrix.parser_protocol_decl_count = snapshot.protocol_decl_count;
  matrix.ast_protocol_decl_count = expected_snapshot.protocol_decl_count;
  matrix.parser_interface_decl_count = snapshot.interface_decl_count;
  matrix.ast_interface_decl_count = expected_snapshot.interface_decl_count;
  matrix.parser_implementation_decl_count = snapshot.implementation_decl_count;
  matrix.ast_implementation_decl_count = expected_snapshot.implementation_decl_count;
  matrix.parser_function_decl_count = snapshot.function_decl_count;
  matrix.ast_function_decl_count = expected_snapshot.function_decl_count;
  matrix.parser_protocol_property_decl_count = snapshot.protocol_property_decl_count;
  matrix.ast_protocol_property_decl_count = expected_snapshot.protocol_property_decl_count;
  matrix.parser_protocol_method_decl_count = snapshot.protocol_method_decl_count;
  matrix.ast_protocol_method_decl_count = expected_snapshot.protocol_method_decl_count;
  matrix.parser_protocol_class_method_decl_count = snapshot.protocol_class_method_decl_count;
  matrix.ast_protocol_class_method_decl_count = expected_snapshot.protocol_class_method_decl_count;
  matrix.parser_protocol_instance_method_decl_count = snapshot.protocol_instance_method_decl_count;
  matrix.ast_protocol_instance_method_decl_count = expected_snapshot.protocol_instance_method_decl_count;
  matrix.parser_interface_property_decl_count = snapshot.interface_property_decl_count;
  matrix.ast_interface_property_decl_count = expected_snapshot.interface_property_decl_count;
  matrix.parser_interface_method_decl_count = snapshot.interface_method_decl_count;
  matrix.ast_interface_method_decl_count = expected_snapshot.interface_method_decl_count;
  matrix.parser_interface_class_method_decl_count = snapshot.interface_class_method_decl_count;
  matrix.ast_interface_class_method_decl_count = expected_snapshot.interface_class_method_decl_count;
  matrix.parser_interface_instance_method_decl_count = snapshot.interface_instance_method_decl_count;
  matrix.ast_interface_instance_method_decl_count = expected_snapshot.interface_instance_method_decl_count;
  matrix.parser_implementation_property_decl_count = snapshot.implementation_property_decl_count;
  matrix.ast_implementation_property_decl_count = expected_snapshot.implementation_property_decl_count;
  matrix.parser_implementation_method_decl_count = snapshot.implementation_method_decl_count;
  matrix.ast_implementation_method_decl_count = expected_snapshot.implementation_method_decl_count;
  matrix.parser_implementation_class_method_decl_count = snapshot.implementation_class_method_decl_count;
  matrix.ast_implementation_class_method_decl_count = expected_snapshot.implementation_class_method_decl_count;
  matrix.parser_implementation_instance_method_decl_count = snapshot.implementation_instance_method_decl_count;
  matrix.ast_implementation_instance_method_decl_count = expected_snapshot.implementation_instance_method_decl_count;
  matrix.parser_interface_category_decl_count = snapshot.interface_category_decl_count;
  matrix.ast_interface_category_decl_count = expected_snapshot.interface_category_decl_count;
  matrix.parser_implementation_category_decl_count = snapshot.implementation_category_decl_count;
  matrix.ast_implementation_category_decl_count = expected_snapshot.implementation_category_decl_count;
  matrix.parser_function_prototype_count = snapshot.function_prototype_count;
  matrix.ast_function_prototype_count = expected_snapshot.function_prototype_count;
  matrix.parser_function_pure_count = snapshot.function_pure_count;
  matrix.ast_function_pure_count = expected_snapshot.function_pure_count;
  matrix.parser_ast_shape_fingerprint = snapshot.ast_shape_fingerprint;
  matrix.ast_shape_fingerprint = expected_snapshot.ast_shape_fingerprint;
  matrix.parser_ast_top_level_layout_fingerprint = snapshot.ast_top_level_layout_fingerprint;
  const std::uint64_t ast_top_level_layout_fingerprint =
      expected_snapshot.ast_top_level_layout_fingerprint;
  matrix.ast_top_level_layout_fingerprint = ast_top_level_layout_fingerprint;
  matrix.parser_contract_snapshot_fingerprint =
      BuildObjc3ParserContractSnapshotFingerprint(snapshot);
  matrix.expected_parser_contract_snapshot_fingerprint =
      BuildObjc3ParserContractSnapshotFingerprint(expected_snapshot);

  std::size_t top_level_decl_count_from_buckets = 0u;
  const bool top_level_decl_buckets_consistent =
      TryBuildObjc3ParserContractTopLevelCountFromDeclBuckets(snapshot, top_level_decl_count_from_buckets) &&
      snapshot.top_level_declaration_count == top_level_decl_count_from_buckets;
  matrix.top_level_declaration_count_matches =
      snapshot.top_level_declaration_count == expected_snapshot.top_level_declaration_count &&
      top_level_decl_buckets_consistent;
  matrix.global_decl_count_matches =
      snapshot.global_decl_count == expected_snapshot.global_decl_count;
  matrix.protocol_decl_count_matches =
      snapshot.protocol_decl_count == expected_snapshot.protocol_decl_count;
  matrix.interface_decl_count_matches =
      snapshot.interface_decl_count == expected_snapshot.interface_decl_count;
  matrix.implementation_decl_count_matches =
      snapshot.implementation_decl_count == expected_snapshot.implementation_decl_count;
  matrix.function_decl_count_matches =
      snapshot.function_decl_count == expected_snapshot.function_decl_count;
  matrix.protocol_property_decl_count_matches =
      snapshot.protocol_property_decl_count == expected_snapshot.protocol_property_decl_count;
  matrix.protocol_method_decl_count_matches =
      snapshot.protocol_method_decl_count == expected_snapshot.protocol_method_decl_count;
  matrix.protocol_class_method_decl_count_matches =
      snapshot.protocol_class_method_decl_count ==
      expected_snapshot.protocol_class_method_decl_count;
  matrix.protocol_instance_method_decl_count_matches =
      snapshot.protocol_instance_method_decl_count ==
      expected_snapshot.protocol_instance_method_decl_count;
  matrix.interface_property_decl_count_matches =
      snapshot.interface_property_decl_count == expected_snapshot.interface_property_decl_count;
  matrix.interface_method_decl_count_matches =
      snapshot.interface_method_decl_count == expected_snapshot.interface_method_decl_count;
  matrix.interface_class_method_decl_count_matches =
      snapshot.interface_class_method_decl_count ==
      expected_snapshot.interface_class_method_decl_count;
  matrix.interface_instance_method_decl_count_matches =
      snapshot.interface_instance_method_decl_count ==
      expected_snapshot.interface_instance_method_decl_count;
  matrix.implementation_property_decl_count_matches =
      snapshot.implementation_property_decl_count ==
      expected_snapshot.implementation_property_decl_count;
  matrix.implementation_method_decl_count_matches =
      snapshot.implementation_method_decl_count ==
      expected_snapshot.implementation_method_decl_count;
  matrix.implementation_class_method_decl_count_matches =
      snapshot.implementation_class_method_decl_count ==
      expected_snapshot.implementation_class_method_decl_count;
  matrix.implementation_instance_method_decl_count_matches =
      snapshot.implementation_instance_method_decl_count ==
      expected_snapshot.implementation_instance_method_decl_count;
  matrix.interface_category_decl_count_matches =
      snapshot.interface_category_decl_count ==
      expected_snapshot.interface_category_decl_count;
  matrix.implementation_category_decl_count_matches =
      snapshot.implementation_category_decl_count ==
      expected_snapshot.implementation_category_decl_count;
  matrix.function_prototype_count_matches =
      snapshot.function_prototype_count == expected_snapshot.function_prototype_count;
  matrix.function_pure_count_matches =
      snapshot.function_pure_count == expected_snapshot.function_pure_count;
  matrix.ast_shape_fingerprint_matches =
      snapshot.ast_shape_fingerprint == expected_snapshot.ast_shape_fingerprint;
  matrix.ast_top_level_layout_fingerprint_matches =
      snapshot.ast_top_level_layout_fingerprint == ast_top_level_layout_fingerprint;
  matrix.parser_contract_snapshot_fingerprint_matches =
      matrix.parser_contract_snapshot_fingerprint ==
      matrix.expected_parser_contract_snapshot_fingerprint;
  const bool parser_diagnostic_budget_consistent =
      snapshot.token_count == 0u || snapshot.parser_diagnostic_count <= snapshot.token_count;
  const bool parser_token_top_level_budget_consistent =
      snapshot.token_count == 0u || snapshot.token_count >= snapshot.top_level_declaration_count;
  const bool parser_subset_count_consistent =
      AreObjc3ParserMethodDeclBucketsConsistent(
          snapshot.protocol_class_method_decl_count,
          snapshot.protocol_instance_method_decl_count,
          snapshot.protocol_method_decl_count) &&
      AreObjc3ParserMethodDeclBucketsConsistent(
          snapshot.interface_class_method_decl_count,
          snapshot.interface_instance_method_decl_count,
          snapshot.interface_method_decl_count) &&
      AreObjc3ParserMethodDeclBucketsConsistent(
          snapshot.implementation_class_method_decl_count,
          snapshot.implementation_instance_method_decl_count,
          snapshot.implementation_method_decl_count) &&
      snapshot.interface_category_decl_count <= snapshot.interface_decl_count &&
      snapshot.implementation_category_decl_count <= snapshot.implementation_decl_count &&
      snapshot.function_prototype_count <= snapshot.function_decl_count &&
      snapshot.function_pure_count <= snapshot.function_decl_count;
  matrix.parser_diagnostic_budget_consistent = parser_diagnostic_budget_consistent;
  matrix.parser_token_top_level_budget_consistent = parser_token_top_level_budget_consistent;
  matrix.parser_subset_count_consistent = parser_subset_count_consistent;
  matrix.parser_contract_snapshot_deterministic = snapshot.deterministic_handoff;
  matrix.parser_recovery_replay_ready = snapshot.parser_recovery_replay_ready;
  matrix.deterministic =
      matrix.top_level_declaration_count_matches &&
      matrix.global_decl_count_matches &&
      matrix.protocol_decl_count_matches &&
      matrix.interface_decl_count_matches &&
      matrix.implementation_decl_count_matches &&
      matrix.function_decl_count_matches &&
      matrix.protocol_property_decl_count_matches &&
      matrix.protocol_method_decl_count_matches &&
      matrix.protocol_class_method_decl_count_matches &&
      matrix.protocol_instance_method_decl_count_matches &&
      matrix.interface_property_decl_count_matches &&
      matrix.interface_method_decl_count_matches &&
      matrix.interface_class_method_decl_count_matches &&
      matrix.interface_instance_method_decl_count_matches &&
      matrix.implementation_property_decl_count_matches &&
      matrix.implementation_method_decl_count_matches &&
      matrix.implementation_class_method_decl_count_matches &&
      matrix.implementation_instance_method_decl_count_matches &&
      matrix.interface_category_decl_count_matches &&
      matrix.implementation_category_decl_count_matches &&
      matrix.function_prototype_count_matches &&
      matrix.function_pure_count_matches &&
      matrix.ast_shape_fingerprint_matches &&
      matrix.ast_top_level_layout_fingerprint_matches &&
      matrix.parser_contract_snapshot_fingerprint_matches &&
      matrix.parser_diagnostic_budget_consistent &&
      matrix.parser_token_top_level_budget_consistent &&
      matrix.parser_subset_count_consistent &&
      matrix.parser_contract_snapshot_deterministic &&
      matrix.parser_recovery_replay_ready;
  return matrix;
}

inline bool IsObjc3ParserContractSnapshotConsistentWithProgram(
    const Objc3ParserContractSnapshot &snapshot, const Objc3ParsedProgram &program) {
  return BuildObjc3ParserSemaConformanceMatrix(snapshot, program).deterministic;
}

inline Objc3ParserSemaConformanceCorpus BuildObjc3ParserSemaConformanceCorpus(
    const Objc3ParserSemaConformanceMatrix &matrix) {
  Objc3ParserSemaConformanceCorpus corpus;
  corpus.has_top_level_declaration_count_case = true;
  corpus.has_snapshot_fingerprint_case = true;
  corpus.has_diagnostic_budget_case = true;
  corpus.has_subset_count_case = true;
  corpus.has_recovery_replay_case = true;
  corpus.top_level_declaration_count_case_passed =
      matrix.top_level_declaration_count_matches;
  corpus.snapshot_fingerprint_case_passed =
      matrix.parser_contract_snapshot_fingerprint_matches;
  corpus.diagnostic_budget_case_passed =
      matrix.parser_diagnostic_budget_consistent &&
      matrix.parser_token_top_level_budget_consistent;
  corpus.subset_count_case_passed = matrix.parser_subset_count_consistent;
  corpus.recovery_replay_case_passed =
      matrix.parser_contract_snapshot_deterministic &&
      matrix.parser_recovery_replay_ready;
  corpus.required_case_count =
      static_cast<std::size_t>(corpus.has_top_level_declaration_count_case) +
      static_cast<std::size_t>(corpus.has_snapshot_fingerprint_case) +
      static_cast<std::size_t>(corpus.has_diagnostic_budget_case) +
      static_cast<std::size_t>(corpus.has_subset_count_case) +
      static_cast<std::size_t>(corpus.has_recovery_replay_case);
  corpus.passed_case_count =
      static_cast<std::size_t>(corpus.top_level_declaration_count_case_passed) +
      static_cast<std::size_t>(corpus.snapshot_fingerprint_case_passed) +
      static_cast<std::size_t>(corpus.diagnostic_budget_case_passed) +
      static_cast<std::size_t>(corpus.subset_count_case_passed) +
      static_cast<std::size_t>(corpus.recovery_replay_case_passed);
  corpus.failed_case_count =
      corpus.required_case_count >= corpus.passed_case_count
          ? (corpus.required_case_count - corpus.passed_case_count)
          : corpus.required_case_count;
  corpus.deterministic = matrix.deterministic && corpus.required_case_count == 5u &&
                         corpus.passed_case_count == corpus.required_case_count &&
                         corpus.failed_case_count == 0u;
  return corpus;
}
