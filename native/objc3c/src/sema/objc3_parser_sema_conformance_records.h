#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_pass_flow_core_contract.h"

struct Objc3ParserSemaConformanceMatrix {
  std::size_t parser_top_level_declaration_count = 0;
  std::size_t ast_top_level_declaration_count = 0;
  std::size_t parser_global_decl_count = 0;
  std::size_t ast_global_decl_count = 0;
  std::size_t parser_protocol_decl_count = 0;
  std::size_t ast_protocol_decl_count = 0;
  std::size_t parser_interface_decl_count = 0;
  std::size_t ast_interface_decl_count = 0;
  std::size_t parser_implementation_decl_count = 0;
  std::size_t ast_implementation_decl_count = 0;
  std::size_t parser_function_decl_count = 0;
  std::size_t ast_function_decl_count = 0;
  std::size_t parser_protocol_property_decl_count = 0;
  std::size_t ast_protocol_property_decl_count = 0;
  std::size_t parser_protocol_method_decl_count = 0;
  std::size_t ast_protocol_method_decl_count = 0;
  std::size_t parser_protocol_class_method_decl_count = 0;
  std::size_t ast_protocol_class_method_decl_count = 0;
  std::size_t parser_protocol_instance_method_decl_count = 0;
  std::size_t ast_protocol_instance_method_decl_count = 0;
  std::size_t parser_interface_property_decl_count = 0;
  std::size_t ast_interface_property_decl_count = 0;
  std::size_t parser_interface_method_decl_count = 0;
  std::size_t ast_interface_method_decl_count = 0;
  std::size_t parser_interface_class_method_decl_count = 0;
  std::size_t ast_interface_class_method_decl_count = 0;
  std::size_t parser_interface_instance_method_decl_count = 0;
  std::size_t ast_interface_instance_method_decl_count = 0;
  std::size_t parser_implementation_property_decl_count = 0;
  std::size_t ast_implementation_property_decl_count = 0;
  std::size_t parser_implementation_method_decl_count = 0;
  std::size_t ast_implementation_method_decl_count = 0;
  std::size_t parser_implementation_class_method_decl_count = 0;
  std::size_t ast_implementation_class_method_decl_count = 0;
  std::size_t parser_implementation_instance_method_decl_count = 0;
  std::size_t ast_implementation_instance_method_decl_count = 0;
  std::size_t parser_interface_category_decl_count = 0;
  std::size_t ast_interface_category_decl_count = 0;
  std::size_t parser_implementation_category_decl_count = 0;
  std::size_t ast_implementation_category_decl_count = 0;
  std::size_t parser_function_prototype_count = 0;
  std::size_t ast_function_prototype_count = 0;
  std::size_t parser_function_pure_count = 0;
  std::size_t ast_function_pure_count = 0;
  std::uint64_t parser_ast_shape_fingerprint = 0;
  std::uint64_t ast_shape_fingerprint = 0;
  std::uint64_t parser_ast_top_level_layout_fingerprint = 0;
  std::uint64_t ast_top_level_layout_fingerprint = 0;
  std::uint64_t parser_contract_snapshot_fingerprint = 0;
  std::uint64_t expected_parser_contract_snapshot_fingerprint = 0;
  bool top_level_declaration_count_matches = false;
  bool global_decl_count_matches = false;
  bool protocol_decl_count_matches = false;
  bool interface_decl_count_matches = false;
  bool implementation_decl_count_matches = false;
  bool function_decl_count_matches = false;
  bool protocol_property_decl_count_matches = false;
  bool protocol_method_decl_count_matches = false;
  bool protocol_class_method_decl_count_matches = false;
  bool protocol_instance_method_decl_count_matches = false;
  bool interface_property_decl_count_matches = false;
  bool interface_method_decl_count_matches = false;
  bool interface_class_method_decl_count_matches = false;
  bool interface_instance_method_decl_count_matches = false;
  bool implementation_property_decl_count_matches = false;
  bool implementation_method_decl_count_matches = false;
  bool implementation_class_method_decl_count_matches = false;
  bool implementation_instance_method_decl_count_matches = false;
  bool interface_category_decl_count_matches = false;
  bool implementation_category_decl_count_matches = false;
  bool function_prototype_count_matches = false;
  bool function_pure_count_matches = false;
  bool ast_shape_fingerprint_matches = false;
  bool ast_top_level_layout_fingerprint_matches = false;
  bool parser_contract_snapshot_fingerprint_matches = false;
  bool parser_diagnostic_budget_consistent = false;
  bool parser_token_top_level_budget_consistent = false;
  bool parser_subset_count_consistent = false;
  bool parser_contract_snapshot_deterministic = false;
  bool parser_recovery_replay_ready = false;
  bool deterministic = false;
};

struct Objc3ParserSemaConformanceCorpus {
  std::size_t required_case_count = 0;
  std::size_t passed_case_count = 0;
  std::size_t failed_case_count = 0;
  bool has_top_level_declaration_count_case = false;
  bool has_snapshot_fingerprint_case = false;
  bool has_diagnostic_budget_case = false;
  bool has_subset_count_case = false;
  bool has_recovery_replay_case = false;
  bool top_level_declaration_count_case_passed = false;
  bool snapshot_fingerprint_case_passed = false;
  bool diagnostic_budget_case_passed = false;
  bool subset_count_case_passed = false;
  bool recovery_replay_case_passed = false;
  bool deterministic = false;
};

struct Objc3ParserSemaConformanceEvidenceRecord {
  std::string parser_sema_conformance_evidence_owner =
      kObjc3ParserSemaConformanceEvidenceOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string parser_sema_contract_handoff_owner =
      kObjc3ParserSemaContractHandoffOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  std::size_t required_matrix_evidence_count = 30u;
  std::size_t passed_matrix_evidence_count = 0;
  std::size_t required_corpus_case_count = 5u;
  std::size_t passed_corpus_case_count = 0;
  std::size_t failed_corpus_case_count = 0;
  bool conformance_matrix_deterministic = false;
  bool conformance_corpus_deterministic = false;
  bool declaration_count_evidence_ready = false;
  bool member_count_evidence_ready = false;
  bool category_function_evidence_ready = false;
  bool fingerprint_evidence_ready = false;
  bool parser_budget_replay_evidence_ready = false;
  bool corpus_inventory_ready = false;
  bool corpus_cases_passed = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3ParserSemaConformanceEvidenceRecord(
    const Objc3ParserSemaConformanceEvidenceRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.parser_sema_conformance_evidence_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_handoff_owner) &&
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
         record.required_matrix_evidence_count == 30u &&
         record.passed_matrix_evidence_count ==
             record.required_matrix_evidence_count &&
         record.required_corpus_case_count == 5u &&
         record.passed_corpus_case_count == record.required_corpus_case_count &&
         record.failed_corpus_case_count == 0u &&
         record.conformance_matrix_deterministic &&
         record.conformance_corpus_deterministic &&
         record.declaration_count_evidence_ready &&
         record.member_count_evidence_ready &&
         record.category_function_evidence_ready &&
         record.fingerprint_evidence_ready &&
         record.parser_budget_replay_evidence_ready &&
         record.corpus_inventory_ready && record.corpus_cases_passed &&
         record.deterministic;
}
