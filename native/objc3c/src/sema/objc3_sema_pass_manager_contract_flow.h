#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "contracts/objc3_diagnostic_owner_contract.h"
#include "sema/objc3_sema_canonical_literal_contract.h"
#include "sema/objc3_sema_closeout_readiness_contract.h"
#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_pass_flow_core_contract.h"
#include "sema/objc3_sema_pass_flow_diagnostics_contract.h"
#include "sema/objc3_sema_pass_manager_publication_contract.h"

struct Objc3SemaTypedSemanticHandoffRecord {
  std::string typed_semantic_handoff_publication_owner =
      kObjc3SemaTypedSemanticHandoffPublicationOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string type_metadata_publication_owner =
      kObjc3SemaTypeMetadataPublicationOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool type_metadata_identity_handoffs_ready = false;
  bool type_annotation_handoffs_ready = false;
  bool module_boundary_handoffs_ready = false;
  bool concurrency_recovery_handoffs_ready = false;
  bool symbol_dispatch_handoffs_ready = false;
  bool block_dispatch_handoffs_ready = false;
  bool ownership_runtime_handoffs_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaTypedSemanticHandoffRecord(
    const Objc3SemaTypedSemanticHandoffRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.typed_semantic_handoff_publication_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.type_metadata_identity_handoffs_ready &&
         record.type_annotation_handoffs_ready &&
         record.module_boundary_handoffs_ready &&
         record.concurrency_recovery_handoffs_ready &&
         record.symbol_dispatch_handoffs_ready &&
         record.block_dispatch_handoffs_ready &&
         record.ownership_runtime_handoffs_ready && record.deterministic;
}

struct Objc3SemaAtomicVectorMappingPublicationRecord {
  std::string atomic_vector_mapping_publication_owner =
      kObjc3SemaAtomicVectorMappingPublicationOwner;
  std::string integration_surface_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  Objc3AtomicMemoryOrderMappingSummary atomic_memory_order_mapping;
  bool deterministic_atomic_memory_order_mapping = false;
  Objc3VectorTypeLoweringSummary vector_type_lowering;
  bool deterministic_vector_type_lowering = false;
  bool atomic_memory_order_mapping_ready = false;
  bool vector_type_lowering_ready = false;
  bool mapping_summaries_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaAtomicVectorMappingPublicationRecord(
    const Objc3SemaAtomicVectorMappingPublicationRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.atomic_vector_mapping_publication_owner) &&
         Objc3SemaOwnerIsExplicit(record.integration_surface_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.atomic_memory_order_mapping_ready &&
         record.vector_type_lowering_ready &&
         record.mapping_summaries_ready && record.deterministic;
}

struct Objc3SemaTypeMetadataMappingReadinessRecord {
  std::string type_metadata_mapping_readiness_owner =
      kObjc3SemaTypeMetadataMappingReadinessOwner;
  std::string integration_surface_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string type_metadata_publication_owner =
      kObjc3SemaTypeMetadataPublicationOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t globals_total = 0;
  std::size_t functions_total = 0;
  std::size_t interfaces_total = 0;
  std::size_t implementations_total = 0;
  std::size_t type_metadata_global_entries = 0;
  std::size_t type_metadata_function_entries = 0;
  std::size_t type_metadata_interface_entries = 0;
  std::size_t type_metadata_implementation_entries = 0;
  bool type_metadata_publication_ready = false;
  bool type_metadata_handoff_ready = false;
  bool cardinality_consistent = false;
  bool atomic_memory_order_mapping_ready = false;
  bool vector_type_lowering_ready = false;
  bool mapping_summaries_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaTypeMetadataMappingReadinessRecord(
    const Objc3SemaTypeMetadataMappingReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.type_metadata_mapping_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.integration_surface_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.type_metadata_publication_ready &&
         record.type_metadata_handoff_ready &&
         record.cardinality_consistent &&
         record.atomic_memory_order_mapping_ready &&
         record.vector_type_lowering_ready && record.mapping_summaries_ready &&
         record.deterministic;
}

struct Objc3SemaCloseoutSurfaceReadinessRecord {
  std::string closeout_surface_readiness_owner =
      kObjc3SemaCloseoutSurfaceReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string parser_sema_contract_readiness_owner =
      kObjc3ParserSemaContractReadinessOwner;
  std::string parser_sema_conformance_evidence_owner =
      kObjc3ParserSemaConformanceEvidenceOwner;
  std::string parity_closeout_publication_readiness_owner =
      kObjc3SemaParityCloseoutPublicationReadinessOwner;
  std::string parity_validation_owner = kObjc3SemaParityValidationOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool parser_sema_contract_ready = false;
  bool parser_sema_conformance_evidence_ready = false;
  bool diagnostics_publication_ready = false;
  bool pass_flow_recovery_ready = false;
  bool pass_manager_publication_ready = false;
  bool type_metadata_publication_ready = false;
  bool type_metadata_mapping_ready = false;
  bool typed_semantic_handoff_ready = false;
  bool parity_closeout_publication_ready = false;
  bool parity_validation_ready = false;
  bool core_semantic_publication_ready = false;
  bool module_semantic_publication_ready = false;
  bool intermodule_flow_publication_ready = false;
  bool concurrency_publication_ready = false;
  bool unsafe_error_validation_ready = false;
  bool control_binding_validation_ready = false;
  bool async_block_message_validation_ready = false;
  bool dispatch_runtime_arc_validation_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaCloseoutSurfaceReadinessRecord(
    const Objc3SemaCloseoutSurfaceReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.closeout_surface_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_conformance_evidence_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parity_closeout_publication_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.parity_validation_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.parser_sema_contract_ready &&
         record.parser_sema_conformance_evidence_ready &&
         record.diagnostics_publication_ready &&
         record.pass_flow_recovery_ready &&
         record.pass_manager_publication_ready &&
         record.type_metadata_publication_ready &&
         record.type_metadata_mapping_ready &&
         record.typed_semantic_handoff_ready &&
         record.parity_closeout_publication_ready &&
         record.parity_validation_ready &&
         record.core_semantic_publication_ready &&
         record.module_semantic_publication_ready &&
         record.intermodule_flow_publication_ready &&
         record.concurrency_publication_ready &&
         record.unsafe_error_validation_ready &&
         record.control_binding_validation_ready &&
         record.async_block_message_validation_ready &&
         record.dispatch_runtime_arc_validation_ready &&
         record.deterministic;
}

struct Objc3SemaCoreSemanticSummaryReadinessRecord {
  std::string core_semantic_summary_readiness_owner =
      kObjc3SemaCoreSemanticSummaryReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool interface_implementation_symbols_ready = false;
  bool interface_implementation_handoff_ready = false;
  bool protocol_category_composition_symbols_ready = false;
  bool protocol_category_composition_handoff_ready = false;
  bool class_protocol_category_linking_symbols_ready = false;
  bool class_protocol_category_linking_handoff_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaCoreSemanticSummaryReadinessRecord(
    const Objc3SemaCoreSemanticSummaryReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.core_semantic_summary_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.interface_implementation_symbols_ready &&
         record.interface_implementation_handoff_ready &&
         record.protocol_category_composition_symbols_ready &&
         record.protocol_category_composition_handoff_ready &&
         record.class_protocol_category_linking_symbols_ready &&
         record.class_protocol_category_linking_handoff_ready &&
         record.deterministic;
}

struct Objc3SemaSelectorPropertyTypeAnnotationReadinessRecord {
  std::string selector_property_type_annotation_readiness_owner =
      kObjc3SemaSelectorPropertyTypeAnnotationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool selector_normalization_ready = false;
  bool property_attribute_ready = false;
  bool type_annotation_surface_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaSelectorPropertyTypeAnnotationReadinessRecord(
    const Objc3SemaSelectorPropertyTypeAnnotationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.selector_property_type_annotation_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.selector_normalization_ready &&
         record.property_attribute_ready &&
         record.type_annotation_surface_ready && record.deterministic;
}

struct Objc3SemaTypeBoundarySummaryReadinessRecord {
  std::string type_boundary_summary_readiness_owner =
      kObjc3SemaTypeBoundarySummaryReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool lightweight_generic_constraint_ready = false;
  bool nullability_flow_warning_precision_ready = false;
  bool protocol_qualified_object_type_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaTypeBoundarySummaryReadinessRecord(
    const Objc3SemaTypeBoundarySummaryReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.type_boundary_summary_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.lightweight_generic_constraint_ready &&
         record.nullability_flow_warning_precision_ready &&
         record.protocol_qualified_object_type_ready &&
         record.deterministic;
}

struct Objc3SemaModuleTypeAbiSummaryReadinessRecord {
  std::string module_type_abi_summary_readiness_owner =
      kObjc3SemaModuleTypeAbiSummaryReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool variance_bridge_cast_ready = false;
  bool generic_metadata_abi_ready = false;
  bool module_import_graph_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaModuleTypeAbiSummaryReadinessRecord(
    const Objc3SemaModuleTypeAbiSummaryReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.module_type_abi_summary_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.variance_bridge_cast_ready &&
         record.generic_metadata_abi_ready &&
         record.module_import_graph_ready && record.deterministic;
}

struct Objc3SemaModuleBoundarySummaryReadinessRecord {
  std::string module_boundary_summary_readiness_owner =
      kObjc3SemaModuleBoundarySummaryReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool namespace_collision_shadowing_ready = false;
  bool public_private_api_partition_ready = false;
  bool incremental_module_cache_invalidation_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaModuleBoundarySummaryReadinessRecord(
    const Objc3SemaModuleBoundarySummaryReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.module_boundary_summary_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.namespace_collision_shadowing_ready &&
         record.public_private_api_partition_ready &&
         record.incremental_module_cache_invalidation_ready &&
         record.deterministic;
}

struct Objc3SemaIntermoduleFlowSummaryReadinessRecord {
  std::string intermodule_flow_summary_readiness_owner =
      kObjc3SemaIntermoduleFlowSummaryReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool cross_module_conformance_ready = false;
  bool throws_propagation_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaIntermoduleFlowSummaryReadinessRecord(
    const Objc3SemaIntermoduleFlowSummaryReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.intermodule_flow_summary_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.cross_module_conformance_ready &&
         record.throws_propagation_ready && record.deterministic;
}

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
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
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
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
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

struct Objc3ParserSemaHandoffPublicationEvidenceRecord {
  std::string handoff_publication_evidence_owner =
      kObjc3ParserSemaHandoffPublicationEvidenceOwner;
  std::string parser_sema_conformance_evidence_owner =
      kObjc3ParserSemaConformanceEvidenceOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string parser_sema_contract_handoff_owner =
      kObjc3ParserSemaContractHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool conformance_matrix_ready = false;
  bool conformance_corpus_ready = false;
  bool parser_recovery_replay_ready = false;
  bool parser_recovery_replay_case_present = false;
  bool parser_recovery_replay_case_passed = false;
  std::size_t corpus_required_case_count = 0;
  std::size_t corpus_passed_case_count = 0;
  std::size_t corpus_failed_case_count = 0;
  bool corpus_case_counts_ready = false;
  bool parser_recovery_replay_contract_satisfied = false;
  std::string recovery_replay_key;
  bool recovery_replay_key_deterministic = false;
  bool recovery_determinism_hardening_satisfied = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3ParserSemaHandoffPublicationEvidenceRecord(
    const Objc3ParserSemaHandoffPublicationEvidenceRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.handoff_publication_evidence_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_conformance_evidence_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.conformance_matrix_ready && record.conformance_corpus_ready &&
         record.parser_recovery_replay_ready &&
         record.parser_recovery_replay_case_present &&
         record.parser_recovery_replay_case_passed &&
         record.corpus_case_counts_ready &&
         record.parser_recovery_replay_contract_satisfied &&
         record.recovery_replay_key.rfind("sema-pass-recovery:v1:", 0) == 0 &&
         record.recovery_replay_key_deterministic &&
         record.recovery_determinism_hardening_satisfied &&
         record.deterministic;
}

struct Objc3ParserSemaHandoffPublicationTransferRecord {
  std::string handoff_publication_transfer_owner =
      kObjc3ParserSemaHandoffPublicationTransferOwner;
  std::string handoff_publication_evidence_owner =
      kObjc3ParserSemaHandoffPublicationEvidenceOwner;
  std::string handoff_scaffold_readiness_owner =
      kObjc3ParserSemaHandoffScaffoldReadinessOwner;
  std::string parser_sema_contract_readiness_owner =
      kObjc3ParserSemaContractReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string parser_sema_contract_handoff_owner =
      kObjc3ParserSemaContractHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_transfer_count = 19u;
  std::size_t passed_transfer_count = 0;
  std::size_t failed_transfer_count = 0;
  bool owner_record_ready = false;
  bool conformance_matrix_ready = false;
  bool conformance_corpus_ready = false;
  bool performance_quality_guardrails_ready = false;
  bool cross_lane_integration_sync_ready = false;
  bool docs_runbook_sync_ready = false;
  bool release_candidate_replay_ready = false;
  bool advanced_core_shard1_ready = false;
  bool advanced_contract_rejection_shard1_ready = false;
  bool advanced_diagnostics_shard1_ready = false;
  bool advanced_conformance_shard1_ready = false;
  bool advanced_integration_shard1_ready = false;
  bool advanced_performance_shard1_ready = false;
  bool advanced_core_shard2_ready = false;
  bool advanced_contract_rejection_shard2_ready = false;
  bool advanced_diagnostics_shard2_ready = false;
  bool integration_closeout_ready = false;
  bool scaffold_readiness_ready = false;
  bool evidence_record_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3ParserSemaHandoffPublicationTransferRecord(
    const Objc3ParserSemaHandoffPublicationTransferRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.handoff_publication_transfer_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.handoff_publication_evidence_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.handoff_scaffold_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_transfer_count == 19u &&
         record.passed_transfer_count == record.required_transfer_count &&
         record.failed_transfer_count == 0u && record.owner_record_ready &&
         record.conformance_matrix_ready && record.conformance_corpus_ready &&
         record.performance_quality_guardrails_ready &&
         record.cross_lane_integration_sync_ready &&
         record.docs_runbook_sync_ready &&
         record.release_candidate_replay_ready &&
         record.advanced_core_shard1_ready &&
         record.advanced_contract_rejection_shard1_ready &&
         record.advanced_diagnostics_shard1_ready &&
         record.advanced_conformance_shard1_ready &&
         record.advanced_integration_shard1_ready &&
         record.advanced_performance_shard1_ready &&
         record.advanced_core_shard2_ready &&
         record.advanced_contract_rejection_shard2_ready &&
         record.advanced_diagnostics_shard2_ready &&
         record.integration_closeout_ready &&
         record.scaffold_readiness_ready && record.evidence_record_ready &&
         record.deterministic;
}

struct Objc3ParserSemaParityPublicationReadinessRecord {
  std::string parser_sema_parity_publication_readiness_owner =
      kObjc3ParserSemaParityPublicationReadinessOwner;
  std::string handoff_publication_transfer_owner =
      kObjc3ParserSemaHandoffPublicationTransferOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_publication_count = 17u;
  std::size_t passed_publication_count = 0;
  std::size_t failed_publication_count = 0;
  bool handoff_publication_transfer_ready = false;
  bool conformance_matrix_ready = false;
  bool conformance_corpus_ready = false;
  bool performance_quality_guardrails_ready = false;
  bool cross_lane_integration_sync_ready = false;
  bool docs_runbook_sync_ready = false;
  bool release_candidate_replay_ready = false;
  bool advanced_core_shard1_ready = false;
  bool advanced_contract_rejection_shard1_ready = false;
  bool advanced_diagnostics_shard1_ready = false;
  bool advanced_conformance_shard1_ready = false;
  bool advanced_integration_shard1_ready = false;
  bool advanced_performance_shard1_ready = false;
  bool advanced_core_shard2_ready = false;
  bool advanced_contract_rejection_shard2_ready = false;
  bool advanced_diagnostics_shard2_ready = false;
  bool integration_closeout_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3ParserSemaParityPublicationReadinessRecord(
    const Objc3ParserSemaParityPublicationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.parser_sema_parity_publication_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.handoff_publication_transfer_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_publication_count == 17u &&
         record.passed_publication_count == record.required_publication_count &&
         record.failed_publication_count == 0u &&
         record.handoff_publication_transfer_ready &&
         record.conformance_matrix_ready && record.conformance_corpus_ready &&
         record.performance_quality_guardrails_ready &&
         record.cross_lane_integration_sync_ready &&
         record.docs_runbook_sync_ready &&
         record.release_candidate_replay_ready &&
         record.advanced_core_shard1_ready &&
         record.advanced_contract_rejection_shard1_ready &&
         record.advanced_diagnostics_shard1_ready &&
         record.advanced_conformance_shard1_ready &&
         record.advanced_integration_shard1_ready &&
         record.advanced_performance_shard1_ready &&
         record.advanced_core_shard2_ready &&
         record.advanced_contract_rejection_shard2_ready &&
         record.advanced_diagnostics_shard2_ready &&
         record.integration_closeout_ready && record.deterministic;
}

struct Objc3SemaCoreSemanticParityPublicationReadinessRecord {
  std::string core_semantic_parity_publication_readiness_owner =
      kObjc3SemaCoreSemanticParityPublicationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_publication_count = 12u;
  std::size_t passed_publication_count = 0;
  std::size_t failed_publication_count = 0;
  bool semantic_diagnostics_ready = false;
  bool type_metadata_handoff_ready = false;
  bool interface_implementation_ready = false;
  bool protocol_category_composition_ready = false;
  bool class_protocol_category_linking_ready = false;
  bool selector_normalization_ready = false;
  bool property_attribute_ready = false;
  bool type_annotation_surface_ready = false;
  bool lightweight_generic_constraint_ready = false;
  bool nullability_flow_warning_precision_ready = false;
  bool protocol_qualified_object_type_ready = false;
  bool variance_bridge_cast_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaCoreSemanticParityPublicationReadinessRecord(
    const Objc3SemaCoreSemanticParityPublicationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.core_semantic_parity_publication_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_publication_count == 12u &&
         record.passed_publication_count == record.required_publication_count &&
         record.failed_publication_count == 0u &&
         record.semantic_diagnostics_ready &&
         record.type_metadata_handoff_ready &&
         record.interface_implementation_ready &&
         record.protocol_category_composition_ready &&
         record.class_protocol_category_linking_ready &&
         record.selector_normalization_ready &&
         record.property_attribute_ready &&
         record.type_annotation_surface_ready &&
         record.lightweight_generic_constraint_ready &&
         record.nullability_flow_warning_precision_ready &&
         record.protocol_qualified_object_type_ready &&
         record.variance_bridge_cast_ready && record.deterministic;
}

struct Objc3SemaModuleSemanticParityPublicationReadinessRecord {
  std::string module_semantic_parity_publication_readiness_owner =
      kObjc3SemaModuleSemanticParityPublicationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_publication_count = 5u;
  std::size_t passed_publication_count = 0;
  std::size_t failed_publication_count = 0;
  bool generic_metadata_abi_ready = false;
  bool module_import_graph_ready = false;
  bool namespace_collision_shadowing_ready = false;
  bool public_private_api_partition_ready = false;
  bool incremental_module_cache_invalidation_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaModuleSemanticParityPublicationReadinessRecord(
    const Objc3SemaModuleSemanticParityPublicationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.module_semantic_parity_publication_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_publication_count == 5u &&
         record.passed_publication_count == record.required_publication_count &&
         record.failed_publication_count == 0u &&
         record.generic_metadata_abi_ready &&
         record.module_import_graph_ready &&
         record.namespace_collision_shadowing_ready &&
         record.public_private_api_partition_ready &&
         record.incremental_module_cache_invalidation_ready &&
         record.deterministic;
}

struct Objc3SemaIntermoduleFlowParityPublicationReadinessRecord {
  std::string intermodule_flow_parity_publication_readiness_owner =
      kObjc3SemaIntermoduleFlowParityPublicationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_publication_count = 2u;
  std::size_t passed_publication_count = 0;
  std::size_t failed_publication_count = 0;
  bool cross_module_conformance_ready = false;
  bool throws_propagation_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaIntermoduleFlowParityPublicationReadinessRecord(
    const Objc3SemaIntermoduleFlowParityPublicationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.intermodule_flow_parity_publication_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_publication_count == 2u &&
         record.passed_publication_count == record.required_publication_count &&
         record.failed_publication_count == 0u &&
         record.cross_module_conformance_ready &&
         record.throws_propagation_ready && record.deterministic;
}

struct Objc3SemaConcurrencyParityPublicationReadinessRecord {
  std::string concurrency_parity_publication_readiness_owner =
      kObjc3SemaConcurrencyParityPublicationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_publication_count = 3u;
  std::size_t passed_publication_count = 0;
  std::size_t failed_publication_count = 0;
  bool actor_isolation_sendability_ready = false;
  bool task_runtime_cancellation_ready = false;
  bool concurrency_replay_race_guard_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaConcurrencyParityPublicationReadinessRecord(
    const Objc3SemaConcurrencyParityPublicationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.concurrency_parity_publication_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_publication_count == 3u &&
         record.passed_publication_count == record.required_publication_count &&
         record.failed_publication_count == 0u &&
         record.actor_isolation_sendability_ready &&
         record.task_runtime_cancellation_ready &&
         record.concurrency_replay_race_guard_ready && record.deterministic;
}

struct Objc3SemaUnsafeErrorParityValidationReadinessRecord {
  std::string unsafe_error_parity_validation_readiness_owner =
      kObjc3SemaUnsafeErrorParityValidationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_validation_count = 5u;
  std::size_t passed_validation_count = 0;
  std::size_t failed_validation_count = 0;
  bool unsafe_pointer_extension_ready = false;
  bool inline_asm_intrinsic_governance_ready = false;
  bool ns_error_bridging_ready = false;
  bool error_diagnostics_recovery_ready = false;
  bool result_like_lowering_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaUnsafeErrorParityValidationReadinessRecord(
    const Objc3SemaUnsafeErrorParityValidationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.unsafe_error_parity_validation_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_validation_count == 5u &&
         record.passed_validation_count == record.required_validation_count &&
         record.failed_validation_count == 0u &&
         record.unsafe_pointer_extension_ready &&
         record.inline_asm_intrinsic_governance_ready &&
         record.ns_error_bridging_ready &&
         record.error_diagnostics_recovery_ready &&
         record.result_like_lowering_ready && record.deterministic;
}

struct Objc3SemaControlBindingParityValidationReadinessRecord {
  std::string control_binding_parity_validation_readiness_owner =
      kObjc3SemaControlBindingParityValidationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_validation_count = 6u;
  std::size_t passed_validation_count = 0;
  std::size_t failed_validation_count = 0;
  bool unwind_cleanup_ready = false;
  bool async_continuation_ready = false;
  bool symbol_graph_scope_resolution_ready = false;
  bool method_lookup_override_conflict_ready = false;
  bool property_synthesis_ivar_binding_ready = false;
  bool id_class_sel_object_pointer_type_checking_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaControlBindingParityValidationReadinessRecord(
    const Objc3SemaControlBindingParityValidationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.control_binding_parity_validation_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_validation_count == 6u &&
         record.passed_validation_count == record.required_validation_count &&
         record.failed_validation_count == 0u &&
         record.unwind_cleanup_ready && record.async_continuation_ready &&
         record.symbol_graph_scope_resolution_ready &&
         record.method_lookup_override_conflict_ready &&
         record.property_synthesis_ivar_binding_ready &&
         record.id_class_sel_object_pointer_type_checking_ready &&
         record.deterministic;
}

struct Objc3SemaAsyncBlockMessageParityValidationReadinessRecord {
  std::string async_block_message_parity_validation_readiness_owner =
      kObjc3SemaAsyncBlockMessageParityValidationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_validation_count = 7u;
  std::size_t passed_validation_count = 0;
  std::size_t failed_validation_count = 0;
  bool await_lowering_suspension_state_lowering_ready = false;
  bool block_literal_capture_semantics_ready = false;
  bool block_abi_invoke_trampoline_ready = false;
  bool block_storage_escape_ready = false;
  bool block_copy_dispose_ready = false;
  bool block_determinism_perf_baseline_ready = false;
  bool message_send_selector_lowering_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaAsyncBlockMessageParityValidationReadinessRecord(
    const Objc3SemaAsyncBlockMessageParityValidationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.async_block_message_parity_validation_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_validation_count == 7u &&
         record.passed_validation_count == record.required_validation_count &&
         record.failed_validation_count == 0u &&
         record.await_lowering_suspension_state_lowering_ready &&
         record.block_literal_capture_semantics_ready &&
         record.block_abi_invoke_trampoline_ready &&
         record.block_storage_escape_ready && record.block_copy_dispose_ready &&
         record.block_determinism_perf_baseline_ready &&
         record.message_send_selector_lowering_ready && record.deterministic;
}

struct Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord {
  std::string dispatch_runtime_arc_parity_validation_readiness_owner =
      kObjc3SemaDispatchRuntimeArcParityValidationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::size_t required_validation_count = 8u;
  std::size_t passed_validation_count = 0;
  std::size_t failed_validation_count = 0;
  bool dispatch_abi_marshalling_ready = false;
  bool nil_receiver_semantics_foldability_ready = false;
  bool super_dispatch_method_family_ready = false;
  bool runtime_link_host_link_ready = false;
  bool retain_release_operation_ready = false;
  bool weak_unowned_semantics_ready = false;
  bool arc_diagnostics_fixit_ready = false;
  bool autoreleasepool_scope_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaDispatchRuntimeArcParityValidationReadinessRecord(
    const Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.dispatch_runtime_arc_parity_validation_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.required_validation_count == 8u &&
         record.passed_validation_count == record.required_validation_count &&
         record.failed_validation_count == 0u &&
         record.dispatch_abi_marshalling_ready &&
         record.nil_receiver_semantics_foldability_ready &&
         record.super_dispatch_method_family_ready &&
         record.runtime_link_host_link_ready &&
         record.retain_release_operation_ready &&
         record.weak_unowned_semantics_ready &&
         record.arc_diagnostics_fixit_ready &&
         record.autoreleasepool_scope_ready && record.deterministic;
}

struct Objc3ParserSemaHandoffScaffoldReadinessRecord {
  std::string handoff_scaffold_readiness_owner =
      kObjc3ParserSemaHandoffScaffoldReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string parser_sema_contract_handoff_owner =
      kObjc3ParserSemaContractHandoffOwner;
  std::string parser_sema_conformance_evidence_owner =
      kObjc3ParserSemaConformanceEvidenceOwner;
  std::string parser_sema_contract_readiness_owner =
      kObjc3ParserSemaContractReadinessOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool owner_record_ready = false;
  bool snapshot_evidence_ready = false;
  bool snapshot_normalization_ready = false;
  bool canonical_rejection_ready = false;
  bool conformance_evidence_ready = false;
  bool parser_contract_readiness_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3ParserSemaHandoffScaffoldReadinessRecord(
    const Objc3ParserSemaHandoffScaffoldReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.handoff_scaffold_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_conformance_evidence_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_readiness_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.owner_record_ready && record.snapshot_evidence_ready &&
         record.snapshot_normalization_ready &&
         record.canonical_rejection_ready &&
         record.conformance_evidence_ready &&
         record.parser_contract_readiness_ready && record.deterministic;
}

struct Objc3ParserSemaPerformanceQualityGuardrails {
  std::size_t conformance_matrix_builder_max_lines = 0;
  std::size_t conformance_corpus_builder_max_lines = 0;
  std::size_t handoff_scaffold_builder_max_lines = 0;
  bool conformance_matrix_builder_budget_guarded = false;
  bool conformance_corpus_builder_budget_guarded = false;
  bool handoff_scaffold_builder_budget_guarded = false;
  bool matrix_diagnostic_budget_consistent = false;
  bool matrix_token_top_level_budget_consistent = false;
  bool matrix_subset_budget_consistent = false;
  bool corpus_case_budget_consistent = false;
  std::size_t required_guardrail_count = 0;
  std::size_t passed_guardrail_count = 0;
  std::size_t failed_guardrail_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaCrossLaneIntegrationSync {
  bool matrix_consistent = false;
  bool corpus_consistent = false;
  bool performance_quality_guardrails_consistent = false;
  bool pass_manager_contract_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaDocsRunbookSync {
  bool cross_lane_integration_sync_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool parity_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaReleaseCandidateReplayDryRun {
  bool docs_runbook_sync_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool replay_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedCoreShard1 {
  bool release_candidate_replay_dry_run_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedContractRejectionShard1 {
  bool advanced_core_shard1_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedDiagnosticsShard1 {
  bool advanced_contract_rejection_shard1_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedConformanceShard1 {
  bool advanced_diagnostics_shard1_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedIntegrationShard1 {
  bool advanced_conformance_shard1_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedPerformanceShard1 {
  bool advanced_integration_shard1_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedCoreShard2 {
  bool advanced_performance_shard1_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedContractRejectionShard2 {
  bool advanced_core_shard2_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaAdvancedDiagnosticsShard2 {
  bool advanced_contract_rejection_shard2_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool shard_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaIntegrationCloseoutSignoff {
  bool advanced_diagnostics_shard2_ready = false;
  bool pass_manager_contract_surface_sync = false;
  bool gate_signoff_surface_sync = false;
  std::size_t required_sync_count = 0;
  std::size_t passed_sync_count = 0;
  std::size_t failed_sync_count = 0;
  bool deterministic = false;
};

struct Objc3ParserSemaContractReadinessRecord {
  std::string parser_sema_contract_readiness_owner =
      kObjc3ParserSemaContractReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string parser_sema_contract_handoff_owner =
      kObjc3ParserSemaContractHandoffOwner;
  std::string parity_validation_owner = kObjc3SemaParityValidationOwner;
  std::string owner_model = kObjc3SemaNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  bool conformance_evidence_ready = false;
  bool conformance_matrix_ready = false;
  bool conformance_corpus_ready = false;
  bool performance_quality_guardrails_ready = false;
  bool cross_lane_integration_sync_ready = false;
  bool docs_runbook_sync_ready = false;
  bool release_candidate_replay_dry_run_ready = false;
  bool advanced_core_shard1_ready = false;
  bool advanced_contract_rejection_shard1_ready = false;
  bool advanced_diagnostics_shard1_ready = false;
  bool advanced_conformance_shard1_ready = false;
  bool advanced_integration_shard1_ready = false;
  bool advanced_performance_shard1_ready = false;
  bool advanced_core_shard2_ready = false;
  bool advanced_contract_rejection_shard2_ready = false;
  bool advanced_diagnostics_shard2_ready = false;
  bool integration_closeout_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3ParserSemaContractReadinessRecord(
    const Objc3ParserSemaContractReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(record.parity_validation_owner) &&
         record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         record.strict_no_fallback && record.strict_no_compatibility &&
         record.conformance_evidence_ready &&
         record.conformance_matrix_ready && record.conformance_corpus_ready &&
         record.performance_quality_guardrails_ready &&
         record.cross_lane_integration_sync_ready &&
         record.docs_runbook_sync_ready &&
         record.release_candidate_replay_dry_run_ready &&
         record.advanced_core_shard1_ready &&
         record.advanced_contract_rejection_shard1_ready &&
         record.advanced_diagnostics_shard1_ready &&
         record.advanced_conformance_shard1_ready &&
         record.advanced_integration_shard1_ready &&
         record.advanced_performance_shard1_ready &&
         record.advanced_core_shard2_ready &&
         record.advanced_contract_rejection_shard2_ready &&
         record.advanced_diagnostics_shard2_ready &&
         record.integration_closeout_ready && record.deterministic;
}

struct Objc3SemaParityContractSurface {
  Objc3ParserSemaConformanceMatrix parser_sema_conformance_matrix;
  Objc3ParserSemaConformanceCorpus parser_sema_conformance_corpus;
  Objc3ParserSemaConformanceEvidenceRecord
      parser_sema_conformance_evidence_record;
  Objc3ParserSemaPerformanceQualityGuardrails parser_sema_performance_quality_guardrails;
  Objc3ParserSemaCrossLaneIntegrationSync parser_sema_cross_lane_integration_sync;
  Objc3ParserSemaDocsRunbookSync parser_sema_docs_runbook_sync;
  Objc3ParserSemaReleaseCandidateReplayDryRun parser_sema_release_candidate_replay_dry_run;
  Objc3ParserSemaAdvancedCoreShard1 parser_sema_advanced_core_shard1;
  Objc3ParserSemaAdvancedContractRejectionShard1 parser_sema_advanced_contract_rejection_shard1;
  Objc3ParserSemaAdvancedDiagnosticsShard1 parser_sema_advanced_diagnostics_shard1;
  Objc3ParserSemaAdvancedConformanceShard1 parser_sema_advanced_conformance_shard1;
  Objc3ParserSemaAdvancedIntegrationShard1 parser_sema_advanced_integration_shard1;
  Objc3ParserSemaAdvancedPerformanceShard1 parser_sema_advanced_performance_shard1;
  Objc3ParserSemaAdvancedCoreShard2 parser_sema_advanced_core_shard2;
  Objc3ParserSemaAdvancedContractRejectionShard2 parser_sema_advanced_contract_rejection_shard2;
  Objc3ParserSemaAdvancedDiagnosticsShard2 parser_sema_advanced_diagnostics_shard2;
  Objc3ParserSemaIntegrationCloseoutSignoff parser_sema_integration_closeout_signoff;
  Objc3ParserSemaHandoffPublicationTransferRecord
      parser_sema_handoff_publication_transfer_record;
  Objc3ParserSemaParityPublicationReadinessRecord
      parser_sema_parity_publication_readiness_record;
  Objc3SemaCoreSemanticParityPublicationReadinessRecord
      core_semantic_parity_publication_readiness_record;
  Objc3SemaCoreSemanticSummaryReadinessRecord
      core_semantic_summary_readiness_record;
  Objc3SemaSelectorPropertyTypeAnnotationReadinessRecord
      selector_property_type_annotation_readiness_record;
  Objc3SemaTypeBoundarySummaryReadinessRecord
      type_boundary_summary_readiness_record;
  Objc3SemaModuleTypeAbiSummaryReadinessRecord
      module_type_abi_summary_readiness_record;
  Objc3SemaModuleBoundarySummaryReadinessRecord
      module_boundary_summary_readiness_record;
  Objc3SemaIntermoduleFlowSummaryReadinessRecord
      intermodule_flow_summary_readiness_record;
  Objc3SemaModuleSemanticParityPublicationReadinessRecord
      module_semantic_parity_publication_readiness_record;
  Objc3SemaIntermoduleFlowParityPublicationReadinessRecord
      intermodule_flow_parity_publication_readiness_record;
  Objc3SemaConcurrencyParityPublicationReadinessRecord
      concurrency_parity_publication_readiness_record;
  Objc3SemaUnsafeErrorParityValidationReadinessRecord
      unsafe_error_parity_validation_readiness_record;
  Objc3SemaControlBindingParityValidationReadinessRecord
      control_binding_parity_validation_readiness_record;
  Objc3SemaAsyncBlockMessageParityValidationReadinessRecord
      async_block_message_parity_validation_readiness_record;
  Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord
      dispatch_runtime_arc_parity_validation_readiness_record;
  Objc3ParserSemaContractReadinessRecord
      parser_sema_contract_readiness_record;
  Objc3SemaPassFlowSummary sema_pass_flow_summary;
  Objc3SemaDiagnosticsPublicationRecord diagnostics_publication_record;
  Objc3SemaPassFlowRecoveryRecord pass_flow_recovery_record;
  Objc3SemaPassManagerPublicationRecord pass_manager_publication_record;
  Objc3SemaTypeMetadataPublicationRecord type_metadata_publication_record;
  Objc3SemaTypeMetadataMappingReadinessRecord
      type_metadata_mapping_readiness_record;
  Objc3SemaAtomicVectorMappingPublicationRecord
      atomic_vector_mapping_publication_record;
  Objc3SemaTypedSemanticHandoffRecord typed_semantic_handoff_record;
  Objc3SemaParityCloseoutPublicationReadinessRecord
      parity_closeout_publication_readiness_record;
  Objc3SemaParityValidationRecord parity_validation_record;
  Objc3SemaCloseoutSurfaceReadinessRecord closeout_surface_readiness_record;
  Objc3SemaCloseoutSignoffRecord closeout_signoff_record;
  std::array<std::size_t, 3> diagnostics_after_pass = {0, 0, 0};
  std::array<std::size_t, 3> diagnostics_emitted_by_pass = {0, 0, 0};
  std::size_t diagnostics_total = 0;
  std::size_t globals_total = 0;
  std::size_t functions_total = 0;
  std::size_t interfaces_total = 0;
  std::size_t implementations_total = 0;
  std::size_t type_metadata_global_entries = 0;
  std::size_t type_metadata_function_entries = 0;
  std::size_t type_metadata_interface_entries = 0;
  std::size_t type_metadata_implementation_entries = 0;
  std::size_t interface_method_symbols_total = 0;
  std::size_t implementation_method_symbols_total = 0;
  std::size_t linked_implementation_symbols_total = 0;
  std::size_t protocol_composition_sites_total = 0;
  std::size_t protocol_composition_symbols_total = 0;
  std::size_t category_composition_sites_total = 0;
  std::size_t category_composition_symbols_total = 0;
  std::size_t invalid_protocol_composition_sites_total = 0;
  std::size_t selector_normalization_methods_total = 0;
  std::size_t selector_normalization_normalized_methods_total = 0;
  std::size_t selector_normalization_piece_entries_total = 0;
  std::size_t selector_normalization_parameter_piece_entries_total = 0;
  std::size_t selector_normalization_pieceless_methods_total = 0;
  std::size_t selector_normalization_spelling_mismatches_total = 0;
  std::size_t selector_normalization_arity_mismatches_total = 0;
  std::size_t selector_normalization_parameter_linkage_mismatches_total = 0;
  std::size_t selector_normalization_flag_mismatches_total = 0;
  std::size_t selector_normalization_missing_keyword_pieces_total = 0;
  std::size_t property_attribute_properties_total = 0;
  std::size_t property_attribute_entries_total = 0;
  std::size_t property_attribute_readonly_modifiers_total = 0;
  std::size_t property_attribute_readwrite_modifiers_total = 0;
  std::size_t property_attribute_atomic_modifiers_total = 0;
  std::size_t property_attribute_nonatomic_modifiers_total = 0;
  std::size_t property_attribute_copy_modifiers_total = 0;
  std::size_t property_attribute_strong_modifiers_total = 0;
  std::size_t property_attribute_weak_modifiers_total = 0;
  std::size_t property_attribute_assign_modifiers_total = 0;
  std::size_t property_attribute_getter_modifiers_total = 0;
  std::size_t property_attribute_setter_modifiers_total = 0;
  std::size_t property_attribute_invalid_attribute_entries_total = 0;
  std::size_t property_attribute_contract_violations_total = 0;
  std::size_t type_annotation_generic_suffix_sites_total = 0;
  std::size_t type_annotation_pointer_declarator_sites_total = 0;
  std::size_t type_annotation_nullability_suffix_sites_total = 0;
  std::size_t type_annotation_ownership_qualifier_sites_total = 0;
  std::size_t type_annotation_object_pointer_type_sites_total = 0;
  std::size_t type_annotation_invalid_generic_suffix_sites_total = 0;
  std::size_t type_annotation_invalid_pointer_declarator_sites_total = 0;
  std::size_t type_annotation_invalid_nullability_suffix_sites_total = 0;
  std::size_t type_annotation_invalid_ownership_qualifier_sites_total = 0;
  std::size_t lightweight_generic_constraint_sites_total = 0;
  std::size_t lightweight_generic_constraint_generic_suffix_sites_total = 0;
  std::size_t lightweight_generic_constraint_object_pointer_type_sites_total = 0;
  std::size_t lightweight_generic_constraint_terminated_generic_suffix_sites_total = 0;
  std::size_t lightweight_generic_constraint_pointer_declarator_sites_total = 0;
  std::size_t lightweight_generic_constraint_normalized_sites_total = 0;
  std::size_t lightweight_generic_constraint_contract_violation_sites_total = 0;
  std::size_t nullability_flow_sites_total = 0;
  std::size_t nullability_flow_object_pointer_type_sites_total = 0;
  std::size_t nullability_flow_nullability_suffix_sites_total = 0;
  std::size_t nullability_flow_nullable_suffix_sites_total = 0;
  std::size_t nullability_flow_nonnull_suffix_sites_total = 0;
  std::size_t nullability_flow_normalized_sites_total = 0;
  std::size_t nullability_flow_contract_violation_sites_total = 0;
  std::size_t protocol_qualified_object_type_sites_total = 0;
  std::size_t protocol_qualified_object_type_protocol_composition_sites_total = 0;
  std::size_t protocol_qualified_object_type_object_pointer_type_sites_total = 0;
  std::size_t protocol_qualified_object_type_terminated_protocol_composition_sites_total = 0;
  std::size_t protocol_qualified_object_type_pointer_declarator_sites_total = 0;
  std::size_t protocol_qualified_object_type_normalized_protocol_composition_sites_total = 0;
  std::size_t protocol_qualified_object_type_contract_violation_sites_total = 0;
  std::size_t variance_bridge_cast_sites_total = 0;
  std::size_t variance_bridge_cast_protocol_composition_sites_total = 0;
  std::size_t variance_bridge_cast_ownership_qualifier_sites_total = 0;
  std::size_t variance_bridge_cast_object_pointer_type_sites_total = 0;
  std::size_t variance_bridge_cast_pointer_declarator_sites_total = 0;
  std::size_t variance_bridge_cast_normalized_sites_total = 0;
  std::size_t variance_bridge_cast_contract_violation_sites_total = 0;
  std::size_t generic_metadata_abi_sites_total = 0;
  std::size_t generic_metadata_abi_generic_suffix_sites_total = 0;
  std::size_t generic_metadata_abi_protocol_composition_sites_total = 0;
  std::size_t generic_metadata_abi_ownership_qualifier_sites_total = 0;
  std::size_t generic_metadata_abi_object_pointer_type_sites_total = 0;
  std::size_t generic_metadata_abi_pointer_declarator_sites_total = 0;
  std::size_t generic_metadata_abi_normalized_sites_total = 0;
  std::size_t generic_metadata_abi_contract_violation_sites_total = 0;
  std::size_t module_import_graph_sites_total = 0;
  std::size_t module_import_graph_import_edge_candidate_sites_total = 0;
  std::size_t module_import_graph_namespace_segment_sites_total = 0;
  std::size_t module_import_graph_object_pointer_type_sites_total = 0;
  std::size_t module_import_graph_pointer_declarator_sites_total = 0;
  std::size_t module_import_graph_normalized_sites_total = 0;
  std::size_t module_import_graph_contract_violation_sites_total = 0;
  std::size_t namespace_collision_shadowing_sites_total = 0;
  std::size_t namespace_collision_shadowing_namespace_segment_sites_total = 0;
  std::size_t namespace_collision_shadowing_import_edge_candidate_sites_total = 0;
  std::size_t namespace_collision_shadowing_object_pointer_type_sites_total = 0;
  std::size_t namespace_collision_shadowing_pointer_declarator_sites_total = 0;
  std::size_t namespace_collision_shadowing_normalized_sites_total = 0;
  std::size_t namespace_collision_shadowing_contract_violation_sites_total = 0;
  std::size_t public_private_api_partition_sites_total = 0;
  std::size_t public_private_api_partition_namespace_segment_sites_total = 0;
  std::size_t public_private_api_partition_import_edge_candidate_sites_total = 0;
  std::size_t public_private_api_partition_object_pointer_type_sites_total = 0;
  std::size_t public_private_api_partition_pointer_declarator_sites_total = 0;
  std::size_t public_private_api_partition_normalized_sites_total = 0;
  std::size_t public_private_api_partition_contract_violation_sites_total = 0;
  std::size_t incremental_module_cache_invalidation_sites_total = 0;
  std::size_t incremental_module_cache_invalidation_namespace_segment_sites_total = 0;
  std::size_t incremental_module_cache_invalidation_import_edge_candidate_sites_total = 0;
  std::size_t incremental_module_cache_invalidation_object_pointer_type_sites_total = 0;
  std::size_t incremental_module_cache_invalidation_pointer_declarator_sites_total = 0;
  std::size_t incremental_module_cache_invalidation_normalized_sites_total = 0;
  std::size_t incremental_module_cache_invalidation_cache_invalidation_candidate_sites_total = 0;
  std::size_t incremental_module_cache_invalidation_contract_violation_sites_total = 0;
  std::size_t cross_module_conformance_sites_total = 0;
  std::size_t cross_module_conformance_namespace_segment_sites_total = 0;
  std::size_t cross_module_conformance_import_edge_candidate_sites_total = 0;
  std::size_t cross_module_conformance_object_pointer_type_sites_total = 0;
  std::size_t cross_module_conformance_pointer_declarator_sites_total = 0;
  std::size_t cross_module_conformance_normalized_sites_total = 0;
  std::size_t cross_module_conformance_cache_invalidation_candidate_sites_total = 0;
  std::size_t cross_module_conformance_contract_violation_sites_total = 0;
  std::size_t throws_propagation_sites_total = 0;
  std::size_t throws_propagation_namespace_segment_sites_total = 0;
  std::size_t throws_propagation_import_edge_candidate_sites_total = 0;
  std::size_t throws_propagation_object_pointer_type_sites_total = 0;
  std::size_t throws_propagation_pointer_declarator_sites_total = 0;
  std::size_t throws_propagation_normalized_sites_total = 0;
  std::size_t throws_propagation_cache_invalidation_candidate_sites_total = 0;
  std::size_t throws_propagation_contract_violation_sites_total = 0;
  std::size_t async_continuation_sites_total = 0;
  std::size_t async_continuation_async_keyword_sites_total = 0;
  std::size_t async_continuation_async_function_sites_total = 0;
  std::size_t async_continuation_allocation_sites_total = 0;
  std::size_t async_continuation_resume_sites_total = 0;
  std::size_t async_continuation_suspend_sites_total = 0;
  std::size_t async_continuation_state_machine_sites_total = 0;
  std::size_t async_continuation_normalized_sites_total = 0;
  std::size_t async_continuation_gate_blocked_sites_total = 0;
  std::size_t async_continuation_contract_violation_sites_total = 0;
  std::size_t actor_isolation_sendability_sites_total = 0;
  std::size_t actor_isolation_decl_sites_total = 0;
  std::size_t actor_hop_sites_total = 0;
  std::size_t sendable_annotation_sites_total = 0;
  std::size_t non_sendable_crossing_sites_total = 0;
  std::size_t actor_isolation_sendability_isolation_boundary_sites_total = 0;
  std::size_t actor_isolation_sendability_normalized_sites_total = 0;
  std::size_t actor_isolation_sendability_gate_blocked_sites_total = 0;
  std::size_t actor_isolation_sendability_contract_violation_sites_total = 0;
  std::size_t task_runtime_cancellation_sites_total = 0;
  std::size_t task_runtime_cancellation_runtime_hook_sites_total = 0;
  std::size_t task_runtime_cancellation_cancellation_check_sites_total = 0;
  std::size_t task_runtime_cancellation_cancellation_handler_sites_total = 0;
  std::size_t task_runtime_cancellation_suspension_point_sites_total = 0;
  std::size_t task_runtime_cancellation_cancellation_propagation_sites_total =
      0;
  std::size_t task_runtime_cancellation_normalized_sites_total = 0;
  std::size_t task_runtime_cancellation_gate_blocked_sites_total = 0;
  std::size_t task_runtime_cancellation_contract_violation_sites_total = 0;
  std::size_t concurrency_replay_race_guard_sites_total = 0;
  std::size_t concurrency_replay_race_guard_concurrency_replay_sites_total = 0;
  std::size_t concurrency_replay_race_guard_replay_proof_sites_total = 0;
  std::size_t concurrency_replay_race_guard_race_guard_sites_total = 0;
  std::size_t concurrency_replay_race_guard_task_handoff_sites_total = 0;
  std::size_t concurrency_replay_race_guard_actor_isolation_sites_total = 0;
  std::size_t concurrency_replay_race_guard_deterministic_schedule_sites_total = 0;
  std::size_t concurrency_replay_race_guard_guard_blocked_sites_total = 0;
  std::size_t concurrency_replay_race_guard_contract_violation_sites_total = 0;
  std::size_t unsafe_pointer_extension_sites_total = 0;
  std::size_t unsafe_pointer_extension_unsafe_keyword_sites_total = 0;
  std::size_t unsafe_pointer_extension_pointer_arithmetic_sites_total = 0;
  std::size_t unsafe_pointer_extension_raw_pointer_type_sites_total = 0;
  std::size_t unsafe_pointer_extension_unsafe_operation_sites_total = 0;
  std::size_t unsafe_pointer_extension_normalized_sites_total = 0;
  std::size_t unsafe_pointer_extension_gate_blocked_sites_total = 0;
  std::size_t unsafe_pointer_extension_contract_violation_sites_total = 0;
  std::size_t inline_asm_intrinsic_governance_sites_total = 0;
  std::size_t inline_asm_intrinsic_governance_inline_asm_sites_total = 0;
  std::size_t inline_asm_intrinsic_governance_intrinsic_sites_total = 0;
  std::size_t inline_asm_intrinsic_governance_governed_intrinsic_sites_total = 0;
  std::size_t inline_asm_intrinsic_governance_privileged_intrinsic_sites_total = 0;
  std::size_t inline_asm_intrinsic_governance_normalized_sites_total = 0;
  std::size_t inline_asm_intrinsic_governance_gate_blocked_sites_total = 0;
  std::size_t inline_asm_intrinsic_governance_contract_violation_sites_total = 0;
  std::size_t ns_error_bridging_sites_total = 0;
  std::size_t ns_error_bridging_ns_error_parameter_sites_total = 0;
  std::size_t ns_error_bridging_ns_error_out_parameter_sites_total = 0;
  std::size_t ns_error_bridging_ns_error_bridge_path_sites_total = 0;
  std::size_t ns_error_bridging_failable_call_sites_total = 0;
  std::size_t ns_error_bridging_normalized_sites_total = 0;
  std::size_t ns_error_bridging_bridge_boundary_sites_total = 0;
  std::size_t ns_error_bridging_contract_violation_sites_total = 0;
  std::size_t error_diagnostics_recovery_sites_total = 0;
  std::size_t error_diagnostics_recovery_diagnostic_emit_sites_total = 0;
  std::size_t error_diagnostics_recovery_recovery_anchor_sites_total = 0;
  std::size_t error_diagnostics_recovery_recovery_boundary_sites_total = 0;
  std::size_t error_diagnostics_recovery_fail_closed_diagnostic_sites_total = 0;
  std::size_t error_diagnostics_recovery_normalized_sites_total = 0;
  std::size_t error_diagnostics_recovery_gate_blocked_sites_total = 0;
  std::size_t error_diagnostics_recovery_contract_violation_sites_total = 0;
  std::size_t result_like_lowering_sites_total = 0;
  std::size_t result_like_lowering_result_success_sites_total = 0;
  std::size_t result_like_lowering_result_failure_sites_total = 0;
  std::size_t result_like_lowering_result_branch_sites_total = 0;
  std::size_t result_like_lowering_result_payload_sites_total = 0;
  std::size_t result_like_lowering_normalized_sites_total = 0;
  std::size_t result_like_lowering_branch_merge_sites_total = 0;
  std::size_t result_like_lowering_contract_violation_sites_total = 0;
  std::size_t unwind_cleanup_sites_total = 0;
  std::size_t unwind_cleanup_exceptional_exit_sites_total = 0;
  std::size_t unwind_cleanup_action_sites_total = 0;
  std::size_t unwind_cleanup_scope_sites_total = 0;
  std::size_t unwind_cleanup_resume_sites_total = 0;
  std::size_t unwind_cleanup_normalized_sites_total = 0;
  std::size_t unwind_cleanup_fail_closed_sites_total = 0;
  std::size_t unwind_cleanup_contract_violation_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_await_keyword_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_await_suspension_point_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_await_resume_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_await_state_machine_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_await_continuation_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_normalized_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_gate_blocked_sites_total = 0;
  std::size_t await_lowering_suspension_state_lowering_contract_violation_sites_total = 0;
  std::size_t symbol_graph_global_symbol_nodes_total = 0;
  std::size_t symbol_graph_function_symbol_nodes_total = 0;
  std::size_t symbol_graph_interface_symbol_nodes_total = 0;
  std::size_t symbol_graph_implementation_symbol_nodes_total = 0;
  std::size_t symbol_graph_interface_property_symbol_nodes_total = 0;
  std::size_t symbol_graph_implementation_property_symbol_nodes_total = 0;
  std::size_t symbol_graph_interface_method_symbol_nodes_total = 0;
  std::size_t symbol_graph_implementation_method_symbol_nodes_total = 0;
  std::size_t symbol_graph_top_level_scope_symbols_total = 0;
  std::size_t symbol_graph_nested_scope_symbols_total = 0;
  std::size_t symbol_graph_scope_frames_total = 0;
  std::size_t symbol_graph_implementation_interface_resolution_sites_total = 0;
  std::size_t symbol_graph_implementation_interface_resolution_hits_total = 0;
  std::size_t symbol_graph_implementation_interface_resolution_misses_total = 0;
  std::size_t symbol_graph_method_resolution_sites_total = 0;
  std::size_t symbol_graph_method_resolution_hits_total = 0;
  std::size_t symbol_graph_method_resolution_misses_total = 0;
  std::size_t method_lookup_override_conflict_lookup_sites_total = 0;
  std::size_t method_lookup_override_conflict_lookup_hits_total = 0;
  std::size_t method_lookup_override_conflict_lookup_misses_total = 0;
  std::size_t method_lookup_override_conflict_override_sites_total = 0;
  std::size_t method_lookup_override_conflict_override_hits_total = 0;
  std::size_t method_lookup_override_conflict_override_misses_total = 0;
  std::size_t method_lookup_override_conflict_override_conflicts_total = 0;
  std::size_t method_lookup_override_conflict_unresolved_base_interfaces_total = 0;
  std::size_t property_synthesis_ivar_binding_property_synthesis_sites_total = 0;
  std::size_t property_synthesis_ivar_binding_explicit_ivar_bindings_total = 0;
  std::size_t property_synthesis_ivar_binding_default_ivar_bindings_total = 0;
  std::size_t property_synthesis_ivar_binding_ivar_binding_sites_total = 0;
  std::size_t property_synthesis_ivar_binding_ivar_binding_resolved_total = 0;
  std::size_t property_synthesis_ivar_binding_ivar_binding_missing_total = 0;
  std::size_t property_synthesis_ivar_binding_ivar_binding_conflicts_total = 0;
  std::size_t id_class_sel_object_pointer_param_type_sites_total = 0;
  std::size_t id_class_sel_object_pointer_param_id_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_param_class_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_param_sel_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_param_instancetype_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_param_object_pointer_type_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_type_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_id_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_class_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_sel_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_instancetype_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_object_pointer_type_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_type_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_id_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_class_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_sel_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_instancetype_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_object_pointer_type_sites_total = 0;
  std::size_t block_literal_capture_semantics_sites_total = 0;
  std::size_t block_literal_capture_semantics_parameter_entries_total = 0;
  std::size_t block_literal_capture_semantics_capture_entries_total = 0;
  std::size_t block_literal_capture_semantics_body_statement_entries_total = 0;
  std::size_t block_literal_capture_semantics_empty_capture_sites_total = 0;
  std::size_t block_literal_capture_semantics_nondeterministic_capture_sites_total = 0;
  std::size_t block_literal_capture_semantics_non_normalized_sites_total = 0;
  std::size_t block_literal_capture_semantics_contract_violation_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_invoke_argument_slots_total = 0;
  std::size_t block_abi_invoke_trampoline_capture_word_count_total = 0;
  std::size_t block_abi_invoke_trampoline_parameter_entries_total = 0;
  std::size_t block_abi_invoke_trampoline_capture_entries_total = 0;
  std::size_t block_abi_invoke_trampoline_body_statement_entries_total = 0;
  std::size_t block_abi_invoke_trampoline_descriptor_symbolized_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_invoke_symbolized_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_missing_invoke_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_non_normalized_layout_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_contract_violation_sites_total = 0;
  std::size_t block_storage_escape_sites_total = 0;
  std::size_t block_storage_escape_mutable_capture_count_total = 0;
  std::size_t block_storage_escape_byref_slot_count_total = 0;
  std::size_t block_storage_escape_parameter_entries_total = 0;
  std::size_t block_storage_escape_capture_entries_total = 0;
  std::size_t block_storage_escape_body_statement_entries_total = 0;
  std::size_t block_storage_escape_requires_byref_cells_sites_total = 0;
  std::size_t block_storage_escape_escape_analysis_enabled_sites_total = 0;
  std::size_t block_storage_escape_escape_to_heap_sites_total = 0;
  std::size_t block_storage_escape_escape_profile_normalized_sites_total = 0;
  std::size_t block_storage_escape_byref_layout_symbolized_sites_total = 0;
  std::size_t block_storage_escape_contract_violation_sites_total = 0;
  std::size_t block_copy_dispose_sites_total = 0;
  std::size_t block_copy_dispose_mutable_capture_count_total = 0;
  std::size_t block_copy_dispose_byref_slot_count_total = 0;
  std::size_t block_copy_dispose_parameter_entries_total = 0;
  std::size_t block_copy_dispose_capture_entries_total = 0;
  std::size_t block_copy_dispose_body_statement_entries_total = 0;
  std::size_t block_copy_dispose_copy_helper_required_sites_total = 0;
  std::size_t block_copy_dispose_dispose_helper_required_sites_total = 0;
  std::size_t block_copy_dispose_profile_normalized_sites_total = 0;
  std::size_t block_copy_dispose_copy_helper_symbolized_sites_total = 0;
  std::size_t block_copy_dispose_dispose_helper_symbolized_sites_total = 0;
  std::size_t block_copy_dispose_contract_violation_sites_total = 0;
  std::size_t block_determinism_perf_baseline_sites_total = 0;
  std::size_t block_determinism_perf_baseline_weight_total = 0;
  std::size_t block_determinism_perf_baseline_parameter_entries_total = 0;
  std::size_t block_determinism_perf_baseline_capture_entries_total = 0;
  std::size_t block_determinism_perf_baseline_body_statement_entries_total = 0;
  std::size_t block_determinism_perf_baseline_deterministic_capture_sites_total = 0;
  std::size_t block_determinism_perf_baseline_heavy_tier_sites_total = 0;
  std::size_t block_determinism_perf_baseline_normalized_profile_sites_total = 0;
  std::size_t block_determinism_perf_baseline_contract_violation_sites_total = 0;
  std::size_t message_send_selector_lowering_sites_total = 0;
  std::size_t message_send_selector_lowering_unary_form_sites_total = 0;
  std::size_t message_send_selector_lowering_keyword_form_sites_total = 0;
  std::size_t message_send_selector_lowering_symbol_sites_total = 0;
  std::size_t message_send_selector_lowering_piece_entries_total = 0;
  std::size_t message_send_selector_lowering_argument_piece_entries_total = 0;
  std::size_t message_send_selector_lowering_normalized_sites_total = 0;
  std::size_t message_send_selector_lowering_form_mismatch_sites_total = 0;
  std::size_t message_send_selector_lowering_arity_mismatch_sites_total = 0;
  std::size_t message_send_selector_lowering_symbol_mismatch_sites_total = 0;
  std::size_t message_send_selector_lowering_missing_symbol_sites_total = 0;
  std::size_t message_send_selector_lowering_contract_violation_sites_total = 0;
  std::size_t dispatch_abi_marshalling_sites_total = 0;
  std::size_t dispatch_abi_marshalling_receiver_slots_total = 0;
  std::size_t dispatch_abi_marshalling_selector_symbol_slots_total = 0;
  std::size_t dispatch_abi_marshalling_argument_slots_total = 0;
  std::size_t dispatch_abi_marshalling_keyword_argument_slots_total = 0;
  std::size_t dispatch_abi_marshalling_unary_argument_slots_total = 0;
  std::size_t dispatch_abi_marshalling_arity_mismatch_sites_total = 0;
  std::size_t dispatch_abi_marshalling_missing_selector_symbol_sites_total = 0;
  std::size_t dispatch_abi_marshalling_contract_violation_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_receiver_nil_literal_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_enabled_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_foldable_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_non_nil_receiver_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_contract_violation_sites_total = 0;
  std::size_t super_dispatch_method_family_sites_total = 0;
  std::size_t super_dispatch_method_family_receiver_super_identifier_sites_total = 0;
  std::size_t super_dispatch_method_family_enabled_sites_total = 0;
  std::size_t super_dispatch_method_family_requires_class_context_sites_total = 0;
  std::size_t super_dispatch_method_family_init_sites_total = 0;
  std::size_t super_dispatch_method_family_copy_sites_total = 0;
  std::size_t super_dispatch_method_family_mutable_copy_sites_total = 0;
  std::size_t super_dispatch_method_family_new_sites_total = 0;
  std::size_t super_dispatch_method_family_none_sites_total = 0;
  std::size_t super_dispatch_method_family_returns_retained_result_sites_total = 0;
  std::size_t super_dispatch_method_family_returns_related_result_sites_total = 0;
  std::size_t super_dispatch_method_family_contract_violation_sites_total = 0;
  std::size_t runtime_link_host_link_message_send_sites_total = 0;
  std::size_t runtime_link_host_link_required_sites_total = 0;
  std::size_t runtime_link_host_link_elided_sites_total = 0;
  std::size_t runtime_link_host_link_runtime_dispatch_arg_slots_total = 0;
  std::size_t runtime_link_host_link_runtime_dispatch_declaration_parameter_count_total = 0;
  std::size_t runtime_link_host_link_contract_violation_sites_total = 0;
  std::string runtime_link_host_link_runtime_dispatch_symbol;
  bool runtime_link_host_link_default_runtime_dispatch_symbol_binding = true;
  std::size_t retain_release_operation_ownership_qualified_sites_total = 0;
  std::size_t retain_release_operation_retain_insertion_sites_total = 0;
  std::size_t retain_release_operation_release_insertion_sites_total = 0;
  std::size_t retain_release_operation_autorelease_insertion_sites_total = 0;
  std::size_t retain_release_operation_contract_violation_sites_total = 0;
  std::size_t weak_unowned_semantics_ownership_candidate_sites_total = 0;
  std::size_t weak_unowned_semantics_weak_reference_sites_total = 0;
  std::size_t weak_unowned_semantics_unowned_reference_sites_total = 0;
  std::size_t weak_unowned_semantics_unowned_safe_reference_sites_total = 0;
  std::size_t weak_unowned_semantics_conflict_sites_total = 0;
  std::size_t weak_unowned_semantics_contract_violation_sites_total = 0;
  std::size_t ownership_arc_diagnostic_candidate_sites_total = 0;
  std::size_t ownership_arc_fixit_available_sites_total = 0;
  std::size_t ownership_arc_profiled_sites_total = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites_total = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites_total = 0;
  std::size_t ownership_arc_contract_violation_sites_total = 0;
  std::size_t autoreleasepool_scope_sites_total = 0;
  std::size_t autoreleasepool_scope_symbolized_sites_total = 0;
  std::size_t autoreleasepool_scope_contract_violation_sites_total = 0;
  unsigned autoreleasepool_scope_max_depth_total = 0;
  bool diagnostics_accounting_consistent = false;
  bool diagnostics_bus_publish_consistent = false;
  bool diagnostics_canonicalized = false;
  bool diagnostics_hardening_satisfied = false;
  bool pass_flow_recovery_replay_contract_satisfied = false;
  std::string pass_flow_recovery_replay_key;
  bool pass_flow_recovery_replay_key_deterministic = false;
  bool pass_flow_recovery_determinism_hardening_satisfied = false;
  bool diagnostics_after_pass_monotonic = false;
  bool deterministic_parser_sema_conformance_matrix = false;
  bool deterministic_parser_sema_conformance_corpus = false;
  bool deterministic_parser_sema_conformance_evidence_record = false;
  bool deterministic_parser_sema_performance_quality_guardrails = false;
  bool deterministic_parser_sema_cross_lane_integration_sync = false;
  bool deterministic_parser_sema_docs_runbook_sync = false;
  bool deterministic_parser_sema_release_candidate_replay_dry_run = false;
  bool deterministic_parser_sema_advanced_core_shard1 = false;
  bool deterministic_parser_sema_advanced_contract_rejection_shard1 = false;
  bool deterministic_parser_sema_advanced_diagnostics_shard1 = false;
  bool deterministic_parser_sema_advanced_conformance_shard1 = false;
  bool deterministic_parser_sema_advanced_integration_shard1 = false;
  bool deterministic_parser_sema_advanced_performance_shard1 = false;
  bool deterministic_parser_sema_advanced_core_shard2 = false;
  bool deterministic_parser_sema_advanced_contract_rejection_shard2 = false;
  bool deterministic_parser_sema_advanced_diagnostics_shard2 = false;
  bool deterministic_parser_sema_integration_closeout_signoff = false;
  bool deterministic_parser_sema_handoff_publication_transfer_record = false;
  bool deterministic_parser_sema_parity_publication_readiness_record = false;
  bool deterministic_core_semantic_parity_publication_readiness_record = false;
  bool deterministic_core_semantic_summary_readiness_record = false;
  bool deterministic_selector_property_type_annotation_readiness_record = false;
  bool deterministic_type_boundary_summary_readiness_record = false;
  bool deterministic_module_type_abi_summary_readiness_record = false;
  bool deterministic_module_boundary_summary_readiness_record = false;
  bool deterministic_intermodule_flow_summary_readiness_record = false;
  bool deterministic_module_semantic_parity_publication_readiness_record = false;
  bool deterministic_intermodule_flow_parity_publication_readiness_record = false;
  bool deterministic_concurrency_parity_publication_readiness_record = false;
  bool deterministic_unsafe_error_parity_validation_readiness_record = false;
  bool deterministic_control_binding_parity_validation_readiness_record = false;
  bool deterministic_async_block_message_parity_validation_readiness_record = false;
  bool deterministic_dispatch_runtime_arc_parity_validation_readiness_record = false;
  bool deterministic_parser_sema_contract_readiness_record = false;
  bool deterministic_diagnostics_publication_record = false;
  bool deterministic_pass_flow_recovery_record = false;
  bool deterministic_pass_manager_publication_record = false;
  bool deterministic_type_metadata_publication_record = false;
  bool deterministic_type_metadata_mapping_readiness_record = false;
  bool deterministic_atomic_vector_mapping_publication_record = false;
  bool deterministic_typed_semantic_handoff_record = false;
  bool deterministic_parity_closeout_publication_readiness_record = false;
  bool deterministic_parity_validation_record = false;
  bool deterministic_closeout_surface_readiness_record = false;
  bool deterministic_closeout_signoff_record = false;
  bool deterministic_semantic_diagnostics = false;
  bool deterministic_type_metadata_handoff = false;
  bool deterministic_interface_implementation_handoff = false;
  bool deterministic_protocol_category_composition_handoff = false;
  bool deterministic_class_protocol_category_linking_handoff = false;
  bool deterministic_selector_normalization_handoff = false;
  bool deterministic_property_attribute_handoff = false;
  bool deterministic_type_annotation_surface_handoff = false;
  bool deterministic_lightweight_generic_constraint_handoff = false;
  bool deterministic_nullability_flow_warning_precision_handoff = false;
  bool deterministic_protocol_qualified_object_type_handoff = false;
  bool deterministic_variance_bridge_cast_handoff = false;
  bool deterministic_generic_metadata_abi_handoff = false;
  bool deterministic_module_import_graph_handoff = false;
  bool deterministic_namespace_collision_shadowing_handoff = false;
  bool deterministic_public_private_api_partition_handoff = false;
  bool deterministic_incremental_module_cache_invalidation_handoff = false;
  bool deterministic_cross_module_conformance_handoff = false;
  bool deterministic_throws_propagation_handoff = false;
  bool deterministic_async_continuation_handoff = false;
  bool deterministic_actor_isolation_sendability_handoff = false;
  bool deterministic_task_runtime_cancellation_handoff = false;
  bool deterministic_concurrency_replay_race_guard_handoff = false;
  bool deterministic_unsafe_pointer_extension_handoff = false;
  bool deterministic_inline_asm_intrinsic_governance_handoff = false;
  bool deterministic_ns_error_bridging_handoff = false;
  bool deterministic_error_diagnostics_recovery_handoff = false;
  bool deterministic_result_like_lowering_handoff = false;
  bool deterministic_unwind_cleanup_handoff = false;
  bool deterministic_await_lowering_suspension_state_lowering_handoff = false;
  bool deterministic_symbol_graph_scope_resolution_handoff = false;
  bool deterministic_method_lookup_override_conflict_handoff = false;
  bool deterministic_property_synthesis_ivar_binding_handoff = false;
  bool deterministic_id_class_sel_object_pointer_type_checking_handoff = false;
  bool deterministic_block_literal_capture_semantics_handoff = false;
  bool deterministic_block_abi_invoke_trampoline_handoff = false;
  bool deterministic_block_storage_escape_handoff = false;
  bool deterministic_block_copy_dispose_handoff = false;
  bool deterministic_block_determinism_perf_baseline_handoff = false;
  bool deterministic_message_send_selector_lowering_handoff = false;
  bool deterministic_dispatch_abi_marshalling_handoff = false;
  bool deterministic_nil_receiver_semantics_foldability_handoff = false;
  bool deterministic_super_dispatch_method_family_handoff = false;
  bool deterministic_runtime_link_host_link_handoff = false;
  bool deterministic_retain_release_operation_handoff = false;
  bool deterministic_weak_unowned_semantics_handoff = false;
  bool deterministic_arc_diagnostics_fixit_handoff = false;
  bool deterministic_autoreleasepool_scope_handoff = false;
  Objc3InterfaceImplementationSummary interface_implementation_summary;
  Objc3BootstrapLegalityFailureContractSummary
      bootstrap_legality_failure_contract_summary;
  Objc3BootstrapLegalitySemanticsSummary
      bootstrap_legality_semantics_summary;
  Objc3BootstrapFailureRestartSemanticsSummary
      bootstrap_failure_restart_semantics_summary;
  Objc3CompatibilityStrictnessClaimSemanticsSummary
      compatibility_strictness_claim_semantics_summary;
  Objc3ProtocolCategoryCompositionSummary protocol_category_composition_summary;
  Objc3ClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;
  Objc3SelectorNormalizationSummary selector_normalization_summary;
  Objc3PropertyAttributeSummary property_attribute_summary;
  Objc3TypeAnnotationSurfaceSummary type_annotation_surface_summary;
  Objc3LightweightGenericConstraintSummary lightweight_generic_constraint_summary;
  Objc3NullabilityFlowWarningPrecisionSummary nullability_flow_warning_precision_summary;
  Objc3ProtocolQualifiedObjectTypeSummary protocol_qualified_object_type_summary;
  Objc3VarianceBridgeCastSummary variance_bridge_cast_summary;
  Objc3GenericMetadataAbiSummary generic_metadata_abi_summary;
  Objc3ModuleImportGraphSummary module_import_graph_summary;
  Objc3NamespaceCollisionShadowingSummary namespace_collision_shadowing_summary;
  Objc3PublicPrivateApiPartitionSummary public_private_api_partition_summary;
  Objc3IncrementalModuleCacheInvalidationSummary incremental_module_cache_invalidation_summary;
  Objc3CrossModuleConformanceSummary cross_module_conformance_summary;
  Objc3ThrowsPropagationSummary throws_propagation_summary;
  Objc3AsyncContinuationSummary async_continuation_summary;
  Objc3ActorIsolationSendabilitySummary actor_isolation_sendability_summary;
  Objc3TaskRuntimeCancellationSummary task_runtime_cancellation_summary;
  Objc3ConcurrencyReplayRaceGuardSummary concurrency_replay_race_guard_summary;
  Objc3UnsafePointerExtensionSummary unsafe_pointer_extension_summary;
  Objc3InlineAsmIntrinsicGovernanceSummary inline_asm_intrinsic_governance_summary;
  Objc3NSErrorBridgingSummary ns_error_bridging_summary;
  Objc3ErrorDiagnosticsRecoverySummary error_diagnostics_recovery_summary;
  Objc3ResultLikeLoweringSummary result_like_lowering_summary;
  Objc3UnwindCleanupSummary unwind_cleanup_summary;
  Objc3AwaitLoweringSuspensionStateSummary
      await_lowering_suspension_state_lowering_summary;
  Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;
  Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary;
  Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary;
  Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary;
  Objc3BlockLiteralCaptureSemanticsSummary block_literal_capture_semantics_summary;
  Objc3BlockAbiInvokeTrampolineSemanticsSummary block_abi_invoke_trampoline_semantics_summary;
  Objc3BlockStorageEscapeSemanticsSummary block_storage_escape_semantics_summary;
  Objc3BlockCopyDisposeSemanticsSummary block_copy_dispose_semantics_summary;
  Objc3BlockDeterminismPerfBaselineSummary block_determinism_perf_baseline_summary;
  Objc3MessageSendSelectorLoweringSummary message_send_selector_lowering_summary;
  Objc3DispatchAbiMarshallingSummary dispatch_abi_marshalling_summary;
  Objc3NilReceiverSemanticsFoldabilitySummary nil_receiver_semantics_foldability_summary;
  Objc3SuperDispatchMethodFamilySummary super_dispatch_method_family_summary;
  Objc3RuntimeLinkHostLinkSummary runtime_link_host_link_summary;
  Objc3RetainReleaseOperationSummary retain_release_operation_summary;
  Objc3WeakUnownedSemanticsSummary weak_unowned_semantics_summary;
  Objc3ArcDiagnosticsFixitSummary arc_diagnostics_fixit_summary;
  Objc3AutoreleasePoolScopeSummary autoreleasepool_scope_summary;
  Objc3AtomicMemoryOrderMappingSummary atomic_memory_order_mapping;
  bool deterministic_atomic_memory_order_mapping = false;
  Objc3VectorTypeLoweringSummary vector_type_lowering;
  bool deterministic_vector_type_lowering = false;
  bool ready = false;
};

inline Objc3SemaCoreSemanticSummaryReadinessRecord
BuildObjc3SemaCoreSemanticSummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaCoreSemanticSummaryReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.interface_implementation_symbols_ready =
      surface.interface_implementation_summary.interface_method_symbols ==
          surface.interface_method_symbols_total &&
      surface.interface_implementation_summary.implementation_method_symbols ==
          surface.implementation_method_symbols_total &&
      surface.interface_implementation_summary.linked_implementation_symbols ==
          surface.linked_implementation_symbols_total;
  record.interface_implementation_handoff_ready =
      surface.interface_implementation_summary.deterministic &&
      surface.deterministic_interface_implementation_handoff;
  record.protocol_category_composition_symbols_ready =
      surface.protocol_category_composition_summary.protocol_composition_sites ==
          surface.protocol_composition_sites_total &&
      surface.protocol_category_composition_summary
              .protocol_composition_symbols ==
          surface.protocol_composition_symbols_total &&
      surface.protocol_category_composition_summary.category_composition_sites ==
          surface.category_composition_sites_total &&
      surface.protocol_category_composition_summary
              .category_composition_symbols ==
          surface.category_composition_symbols_total &&
      surface.protocol_category_composition_summary
              .invalid_protocol_composition_sites ==
          surface.invalid_protocol_composition_sites_total &&
      surface.protocol_category_composition_summary
              .invalid_protocol_composition_sites <=
          surface.protocol_category_composition_summary
              .total_composition_sites();
  record.protocol_category_composition_handoff_ready =
      surface.protocol_category_composition_summary.deterministic &&
      surface.deterministic_protocol_category_composition_handoff;
  record.class_protocol_category_linking_symbols_ready =
      surface.class_protocol_category_linking_summary.declared_interfaces ==
          surface.interface_implementation_summary.declared_interfaces &&
      surface.class_protocol_category_linking_summary.resolved_interfaces ==
          surface.interface_implementation_summary.resolved_interfaces &&
      surface.class_protocol_category_linking_summary.declared_implementations ==
          surface.interface_implementation_summary.declared_implementations &&
      surface.class_protocol_category_linking_summary.resolved_implementations ==
          surface.interface_implementation_summary.resolved_implementations &&
      surface.class_protocol_category_linking_summary.interface_method_symbols ==
          surface.interface_method_symbols_total &&
      surface.class_protocol_category_linking_summary
              .implementation_method_symbols ==
          surface.implementation_method_symbols_total &&
      surface.class_protocol_category_linking_summary
              .linked_implementation_symbols ==
          surface.linked_implementation_symbols_total &&
      surface.class_protocol_category_linking_summary
              .protocol_composition_sites ==
          surface.protocol_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .protocol_composition_symbols ==
          surface.protocol_composition_symbols_total &&
      surface.class_protocol_category_linking_summary.category_composition_sites ==
          surface.category_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .category_composition_symbols ==
          surface.category_composition_symbols_total &&
      surface.class_protocol_category_linking_summary
              .invalid_protocol_composition_sites ==
          surface.invalid_protocol_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .invalid_protocol_composition_sites <=
          surface.class_protocol_category_linking_summary.total_composition_sites();
  record.class_protocol_category_linking_handoff_ready =
      surface.class_protocol_category_linking_summary.deterministic &&
      surface.deterministic_class_protocol_category_linking_handoff;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.core_semantic_summary_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.interface_implementation_symbols_ready &&
      record.interface_implementation_handoff_ready &&
      record.protocol_category_composition_symbols_ready &&
      record.protocol_category_composition_handoff_ready &&
      record.class_protocol_category_linking_symbols_ready &&
      record.class_protocol_category_linking_handoff_ready;
  return record;
}

inline Objc3SemaSelectorPropertyTypeAnnotationReadinessRecord
BuildObjc3SemaSelectorPropertyTypeAnnotationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaSelectorPropertyTypeAnnotationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.selector_normalization_ready =
      surface.deterministic_selector_normalization_handoff &&
      surface.selector_normalization_summary.methods_total ==
          surface.selector_normalization_methods_total &&
      surface.selector_normalization_summary.normalized_methods ==
          surface.selector_normalization_normalized_methods_total &&
      surface.selector_normalization_summary.selector_piece_entries ==
          surface.selector_normalization_piece_entries_total &&
      surface.selector_normalization_summary
              .selector_parameter_piece_entries ==
          surface.selector_normalization_parameter_piece_entries_total &&
      surface.selector_normalization_summary.selector_pieceless_methods ==
          surface.selector_normalization_pieceless_methods_total &&
      surface.selector_normalization_summary.selector_spelling_mismatches ==
          surface.selector_normalization_spelling_mismatches_total &&
      surface.selector_normalization_summary.selector_arity_mismatches ==
          surface.selector_normalization_arity_mismatches_total &&
      surface.selector_normalization_summary
              .selector_parameter_linkage_mismatches ==
          surface.selector_normalization_parameter_linkage_mismatches_total &&
      surface.selector_normalization_summary
              .selector_normalization_flag_mismatches ==
          surface.selector_normalization_flag_mismatches_total &&
      surface.selector_normalization_summary.selector_missing_keyword_pieces ==
          surface.selector_normalization_missing_keyword_pieces_total &&
      surface.selector_normalization_summary.selector_parameter_piece_entries <=
          surface.selector_normalization_summary.selector_piece_entries &&
      surface.selector_normalization_summary.normalized_methods <=
          surface.selector_normalization_summary.methods_total &&
      surface.selector_normalization_summary.contract_violations() <=
          surface.selector_normalization_summary.methods_total &&
      surface.selector_normalization_summary.deterministic;
  record.property_attribute_ready =
      surface.deterministic_property_attribute_handoff &&
      surface.property_attribute_summary.properties_total ==
          surface.property_attribute_properties_total &&
      surface.property_attribute_summary.attribute_entries ==
          surface.property_attribute_entries_total &&
      surface.property_attribute_summary.readonly_modifiers ==
          surface.property_attribute_readonly_modifiers_total &&
      surface.property_attribute_summary.readwrite_modifiers ==
          surface.property_attribute_readwrite_modifiers_total &&
      surface.property_attribute_summary.atomic_modifiers ==
          surface.property_attribute_atomic_modifiers_total &&
      surface.property_attribute_summary.nonatomic_modifiers ==
          surface.property_attribute_nonatomic_modifiers_total &&
      surface.property_attribute_summary.copy_modifiers ==
          surface.property_attribute_copy_modifiers_total &&
      surface.property_attribute_summary.strong_modifiers ==
          surface.property_attribute_strong_modifiers_total &&
      surface.property_attribute_summary.weak_modifiers ==
          surface.property_attribute_weak_modifiers_total &&
      surface.property_attribute_summary.assign_modifiers ==
          surface.property_attribute_assign_modifiers_total &&
      surface.property_attribute_summary.getter_modifiers ==
          surface.property_attribute_getter_modifiers_total &&
      surface.property_attribute_summary.setter_modifiers ==
          surface.property_attribute_setter_modifiers_total &&
      surface.property_attribute_summary.invalid_attribute_entries ==
          surface.property_attribute_invalid_attribute_entries_total &&
      surface.property_attribute_summary.property_contract_violations ==
          surface.property_attribute_contract_violations_total &&
      surface.property_attribute_summary.getter_modifiers <=
          surface.property_attribute_summary.properties_total &&
      surface.property_attribute_summary.setter_modifiers <=
          surface.property_attribute_summary.properties_total &&
      surface.property_attribute_summary.deterministic;
  record.type_annotation_surface_ready =
      surface.deterministic_type_annotation_surface_handoff &&
      surface.type_annotation_surface_summary.generic_suffix_sites ==
          surface.type_annotation_generic_suffix_sites_total &&
      surface.type_annotation_surface_summary.pointer_declarator_sites ==
          surface.type_annotation_pointer_declarator_sites_total &&
      surface.type_annotation_surface_summary.nullability_suffix_sites ==
          surface.type_annotation_nullability_suffix_sites_total &&
      surface.type_annotation_surface_summary.ownership_qualifier_sites ==
          surface.type_annotation_ownership_qualifier_sites_total &&
      surface.type_annotation_surface_summary.object_pointer_type_sites ==
          surface.type_annotation_object_pointer_type_sites_total &&
      surface.type_annotation_surface_summary.invalid_generic_suffix_sites ==
          surface.type_annotation_invalid_generic_suffix_sites_total &&
      surface.type_annotation_surface_summary.invalid_pointer_declarator_sites ==
          surface.type_annotation_invalid_pointer_declarator_sites_total &&
      surface.type_annotation_surface_summary.invalid_nullability_suffix_sites ==
          surface.type_annotation_invalid_nullability_suffix_sites_total &&
      surface.type_annotation_surface_summary
              .invalid_ownership_qualifier_sites ==
          surface.type_annotation_invalid_ownership_qualifier_sites_total &&
      surface.type_annotation_surface_summary.invalid_generic_suffix_sites <=
          surface.type_annotation_surface_summary.generic_suffix_sites &&
      surface.type_annotation_surface_summary.invalid_pointer_declarator_sites <=
          surface.type_annotation_surface_summary.pointer_declarator_sites &&
      surface.type_annotation_surface_summary.invalid_nullability_suffix_sites <=
          surface.type_annotation_surface_summary.nullability_suffix_sites &&
      surface.type_annotation_surface_summary
              .invalid_ownership_qualifier_sites <=
          surface.type_annotation_surface_summary.ownership_qualifier_sites &&
      surface.type_annotation_surface_summary.invalid_type_annotation_sites() <=
          surface.type_annotation_surface_summary.total_type_annotation_sites() &&
      surface.type_annotation_surface_summary.deterministic;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.selector_property_type_annotation_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.selector_normalization_ready &&
      record.property_attribute_ready &&
      record.type_annotation_surface_ready;
  return record;
}

inline Objc3SemaTypeBoundarySummaryReadinessRecord
BuildObjc3SemaTypeBoundarySummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaTypeBoundarySummaryReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.lightweight_generic_constraint_ready =
      surface.deterministic_lightweight_generic_constraint_handoff &&
      surface.lightweight_generic_constraint_summary.generic_constraint_sites ==
          surface.lightweight_generic_constraint_sites_total &&
      surface.lightweight_generic_constraint_summary.generic_suffix_sites ==
          surface.lightweight_generic_constraint_generic_suffix_sites_total &&
      surface.lightweight_generic_constraint_summary.object_pointer_type_sites ==
          surface
              .lightweight_generic_constraint_object_pointer_type_sites_total &&
      surface.lightweight_generic_constraint_summary
              .terminated_generic_suffix_sites ==
          surface
              .lightweight_generic_constraint_terminated_generic_suffix_sites_total &&
      surface.lightweight_generic_constraint_summary.pointer_declarator_sites ==
          surface.lightweight_generic_constraint_pointer_declarator_sites_total &&
      surface.lightweight_generic_constraint_summary.normalized_constraint_sites ==
          surface.lightweight_generic_constraint_normalized_sites_total &&
      surface.lightweight_generic_constraint_summary.contract_violation_sites ==
          surface
              .lightweight_generic_constraint_contract_violation_sites_total &&
      surface.lightweight_generic_constraint_summary
              .terminated_generic_suffix_sites <=
          surface.lightweight_generic_constraint_summary.generic_suffix_sites &&
      surface.lightweight_generic_constraint_summary.normalized_constraint_sites <=
          surface.lightweight_generic_constraint_summary
              .generic_constraint_sites &&
      surface.lightweight_generic_constraint_summary.contract_violation_sites <=
          surface.lightweight_generic_constraint_summary.generic_constraint_sites &&
      surface.lightweight_generic_constraint_summary.deterministic;
  record.nullability_flow_warning_precision_ready =
      surface.deterministic_nullability_flow_warning_precision_handoff &&
      surface.nullability_flow_warning_precision_summary.nullability_flow_sites ==
          surface.nullability_flow_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .object_pointer_type_sites ==
          surface.nullability_flow_object_pointer_type_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .nullability_suffix_sites ==
          surface.nullability_flow_nullability_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary.nullable_suffix_sites ==
          surface.nullability_flow_nullable_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary.nonnull_suffix_sites ==
          surface.nullability_flow_nonnull_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary.normalized_sites ==
          surface.nullability_flow_normalized_sites_total &&
      surface.nullability_flow_warning_precision_summary.contract_violation_sites ==
          surface.nullability_flow_contract_violation_sites_total &&
      surface.nullability_flow_warning_precision_summary.normalized_sites <=
          surface.nullability_flow_warning_precision_summary
              .nullability_flow_sites &&
      surface.nullability_flow_warning_precision_summary.contract_violation_sites <=
          surface.nullability_flow_warning_precision_summary
              .nullability_flow_sites &&
      surface.nullability_flow_warning_precision_summary.nullability_suffix_sites ==
          surface.nullability_flow_warning_precision_summary
              .nullable_suffix_sites +
              surface.nullability_flow_warning_precision_summary
                  .nonnull_suffix_sites &&
      surface.nullability_flow_warning_precision_summary.deterministic;
  record.protocol_qualified_object_type_ready =
      surface.deterministic_protocol_qualified_object_type_handoff &&
      surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites ==
          surface.protocol_qualified_object_type_sites_total &&
      surface.protocol_qualified_object_type_summary.protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.object_pointer_type_sites ==
          surface
              .protocol_qualified_object_type_object_pointer_type_sites_total &&
      surface.protocol_qualified_object_type_summary
              .terminated_protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_terminated_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.pointer_declarator_sites ==
          surface
              .protocol_qualified_object_type_pointer_declarator_sites_total &&
      surface.protocol_qualified_object_type_summary
              .normalized_protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_normalized_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.contract_violation_sites ==
          surface
              .protocol_qualified_object_type_contract_violation_sites_total &&
      surface.protocol_qualified_object_type_summary
              .terminated_protocol_composition_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_composition_sites &&
      surface.protocol_qualified_object_type_summary
              .normalized_protocol_composition_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites &&
      surface.protocol_qualified_object_type_summary.contract_violation_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites &&
      surface.protocol_qualified_object_type_summary.deterministic;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.type_boundary_summary_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.lightweight_generic_constraint_ready &&
      record.nullability_flow_warning_precision_ready &&
      record.protocol_qualified_object_type_ready;
  return record;
}

inline Objc3SemaModuleTypeAbiSummaryReadinessRecord
BuildObjc3SemaModuleTypeAbiSummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaModuleTypeAbiSummaryReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.variance_bridge_cast_ready =
      surface.deterministic_variance_bridge_cast_handoff &&
      surface.variance_bridge_cast_summary.variance_bridge_cast_sites ==
          surface.variance_bridge_cast_sites_total &&
      surface.variance_bridge_cast_summary.protocol_composition_sites ==
          surface.variance_bridge_cast_protocol_composition_sites_total &&
      surface.variance_bridge_cast_summary.ownership_qualifier_sites ==
          surface.variance_bridge_cast_ownership_qualifier_sites_total &&
      surface.variance_bridge_cast_summary.object_pointer_type_sites ==
          surface.variance_bridge_cast_object_pointer_type_sites_total &&
      surface.variance_bridge_cast_summary.pointer_declarator_sites ==
          surface.variance_bridge_cast_pointer_declarator_sites_total &&
      surface.variance_bridge_cast_summary.normalized_sites ==
          surface.variance_bridge_cast_normalized_sites_total &&
      surface.variance_bridge_cast_summary.contract_violation_sites ==
          surface.variance_bridge_cast_contract_violation_sites_total &&
      surface.variance_bridge_cast_summary.protocol_composition_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.normalized_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.contract_violation_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.deterministic;
  record.generic_metadata_abi_ready =
      surface.deterministic_generic_metadata_abi_handoff &&
      surface.generic_metadata_abi_summary.generic_metadata_abi_sites ==
          surface.generic_metadata_abi_sites_total &&
      surface.generic_metadata_abi_summary.generic_suffix_sites ==
          surface.generic_metadata_abi_generic_suffix_sites_total &&
      surface.generic_metadata_abi_summary.protocol_composition_sites ==
          surface.generic_metadata_abi_protocol_composition_sites_total &&
      surface.generic_metadata_abi_summary.ownership_qualifier_sites ==
          surface.generic_metadata_abi_ownership_qualifier_sites_total &&
      surface.generic_metadata_abi_summary.object_pointer_type_sites ==
          surface.generic_metadata_abi_object_pointer_type_sites_total &&
      surface.generic_metadata_abi_summary.pointer_declarator_sites ==
          surface.generic_metadata_abi_pointer_declarator_sites_total &&
      surface.generic_metadata_abi_summary.normalized_sites ==
          surface.generic_metadata_abi_normalized_sites_total &&
      surface.generic_metadata_abi_summary.contract_violation_sites ==
          surface.generic_metadata_abi_contract_violation_sites_total &&
      surface.generic_metadata_abi_summary.generic_suffix_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.protocol_composition_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.normalized_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.contract_violation_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.deterministic;
  record.module_import_graph_ready =
      surface.deterministic_module_import_graph_handoff &&
      surface.module_import_graph_summary.module_import_graph_sites ==
          surface.module_import_graph_sites_total &&
      surface.module_import_graph_summary.import_edge_candidate_sites ==
          surface.module_import_graph_import_edge_candidate_sites_total &&
      surface.module_import_graph_summary.namespace_segment_sites ==
          surface.module_import_graph_namespace_segment_sites_total &&
      surface.module_import_graph_summary.object_pointer_type_sites ==
          surface.module_import_graph_object_pointer_type_sites_total &&
      surface.module_import_graph_summary.pointer_declarator_sites ==
          surface.module_import_graph_pointer_declarator_sites_total &&
      surface.module_import_graph_summary.normalized_sites ==
          surface.module_import_graph_normalized_sites_total &&
      surface.module_import_graph_summary.contract_violation_sites ==
          surface.module_import_graph_contract_violation_sites_total &&
      surface.module_import_graph_summary.import_edge_candidate_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.namespace_segment_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.normalized_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.contract_violation_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.deterministic;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.module_type_abi_summary_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.variance_bridge_cast_ready &&
      record.generic_metadata_abi_ready && record.module_import_graph_ready;
  return record;
}

inline Objc3SemaModuleBoundarySummaryReadinessRecord
BuildObjc3SemaModuleBoundarySummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaModuleBoundarySummaryReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.namespace_collision_shadowing_ready =
      surface.deterministic_namespace_collision_shadowing_handoff &&
      surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites ==
          surface.namespace_collision_shadowing_sites_total &&
      surface.namespace_collision_shadowing_summary.namespace_segment_sites ==
          surface
              .namespace_collision_shadowing_namespace_segment_sites_total &&
      surface.namespace_collision_shadowing_summary
              .import_edge_candidate_sites ==
          surface
              .namespace_collision_shadowing_import_edge_candidate_sites_total &&
      surface.namespace_collision_shadowing_summary.object_pointer_type_sites ==
          surface
              .namespace_collision_shadowing_object_pointer_type_sites_total &&
      surface.namespace_collision_shadowing_summary.pointer_declarator_sites ==
          surface
              .namespace_collision_shadowing_pointer_declarator_sites_total &&
      surface.namespace_collision_shadowing_summary.normalized_sites ==
          surface.namespace_collision_shadowing_normalized_sites_total &&
      surface.namespace_collision_shadowing_summary.contract_violation_sites ==
          surface
              .namespace_collision_shadowing_contract_violation_sites_total &&
      surface.namespace_collision_shadowing_summary.namespace_segment_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary
              .import_edge_candidate_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary.normalized_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary.contract_violation_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary.deterministic;
  record.public_private_api_partition_ready =
      surface.deterministic_public_private_api_partition_handoff &&
      surface.public_private_api_partition_summary
              .public_private_api_partition_sites ==
          surface.public_private_api_partition_sites_total &&
      surface.public_private_api_partition_summary.namespace_segment_sites ==
          surface.public_private_api_partition_namespace_segment_sites_total &&
      surface.public_private_api_partition_summary.import_edge_candidate_sites ==
          surface.public_private_api_partition_import_edge_candidate_sites_total &&
      surface.public_private_api_partition_summary.object_pointer_type_sites ==
          surface.public_private_api_partition_object_pointer_type_sites_total &&
      surface.public_private_api_partition_summary.pointer_declarator_sites ==
          surface.public_private_api_partition_pointer_declarator_sites_total &&
      surface.public_private_api_partition_summary.normalized_sites ==
          surface.public_private_api_partition_normalized_sites_total &&
      surface.public_private_api_partition_summary.contract_violation_sites ==
          surface.public_private_api_partition_contract_violation_sites_total &&
      surface.public_private_api_partition_summary.namespace_segment_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.import_edge_candidate_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.normalized_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.contract_violation_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.deterministic;
  record.incremental_module_cache_invalidation_ready =
      surface.deterministic_incremental_module_cache_invalidation_handoff &&
      surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites ==
          surface.incremental_module_cache_invalidation_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .namespace_segment_sites ==
          surface
              .incremental_module_cache_invalidation_namespace_segment_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .import_edge_candidate_sites ==
          surface
              .incremental_module_cache_invalidation_import_edge_candidate_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .object_pointer_type_sites ==
          surface
              .incremental_module_cache_invalidation_object_pointer_type_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .pointer_declarator_sites ==
          surface
              .incremental_module_cache_invalidation_pointer_declarator_sites_total &&
      surface.incremental_module_cache_invalidation_summary.normalized_sites ==
          surface.incremental_module_cache_invalidation_normalized_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .cache_invalidation_candidate_sites ==
          surface
              .incremental_module_cache_invalidation_cache_invalidation_candidate_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .contract_violation_sites ==
          surface
              .incremental_module_cache_invalidation_contract_violation_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .namespace_segment_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary
              .import_edge_candidate_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary.normalized_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary
              .cache_invalidation_candidate_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary
              .contract_violation_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary.deterministic;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.module_boundary_summary_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.namespace_collision_shadowing_ready &&
      record.public_private_api_partition_ready &&
      record.incremental_module_cache_invalidation_ready;
  return record;
}

Objc3SemaTypedSemanticHandoffRecord
BuildObjc3SemaTypedSemanticHandoffRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaAtomicVectorMappingPublicationRecord
BuildObjc3SemaAtomicVectorMappingPublicationRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3AtomicMemoryOrderMappingSummary &atomic_memory_order_mapping,
    bool deterministic_atomic_memory_order_mapping,
    const Objc3VectorTypeLoweringSummary &vector_type_lowering,
    bool deterministic_vector_type_lowering);

Objc3SemaTypeMetadataMappingReadinessRecord
BuildObjc3SemaTypeMetadataMappingReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

bool Objc3ParserSemaSyncCountsReady(
    std::size_t expected_count,
    std::size_t required_count,
    std::size_t passed_count,
    std::size_t failed_count);

std::size_t Objc3SemaEvidenceCount(bool ready);

Objc3ParserSemaParityPublicationReadinessRecord
BuildObjc3ParserSemaParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3ParserSemaHandoffPublicationTransferRecord &transfer_record,
    bool deterministic_transfer_record);

inline Objc3SemaCoreSemanticParityPublicationReadinessRecord
BuildObjc3SemaCoreSemanticParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_semantic_diagnostics,
    bool deterministic_type_metadata_handoff,
    bool deterministic_interface_implementation_handoff,
    bool deterministic_protocol_category_composition_handoff,
    bool deterministic_class_protocol_category_linking_handoff,
    bool deterministic_selector_normalization_handoff,
    bool deterministic_property_attribute_handoff,
    bool deterministic_type_annotation_surface_handoff,
    bool deterministic_lightweight_generic_constraint_handoff,
    bool deterministic_nullability_flow_warning_precision_handoff,
    bool deterministic_protocol_qualified_object_type_handoff,
    bool deterministic_variance_bridge_cast_handoff) {
  Objc3SemaCoreSemanticParityPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.semantic_diagnostics_ready = deterministic_semantic_diagnostics;
  record.type_metadata_handoff_ready = deterministic_type_metadata_handoff;
  record.interface_implementation_ready =
      deterministic_interface_implementation_handoff &&
      surface.interfaces_total == surface.type_metadata_interface_entries &&
      surface.implementations_total ==
          surface.type_metadata_implementation_entries &&
      surface.interface_implementation_summary.resolved_interfaces ==
          surface.type_metadata_interface_entries &&
      surface.interface_implementation_summary.resolved_implementations ==
          surface.type_metadata_implementation_entries;
  record.protocol_category_composition_ready =
      deterministic_protocol_category_composition_handoff &&
      surface.protocol_category_composition_summary.protocol_composition_sites ==
          surface.protocol_composition_sites_total &&
      surface.protocol_category_composition_summary
              .protocol_composition_symbols ==
          surface.protocol_composition_symbols_total &&
      surface.protocol_category_composition_summary.category_composition_sites ==
          surface.category_composition_sites_total &&
      surface.protocol_category_composition_summary
              .category_composition_symbols ==
          surface.category_composition_symbols_total &&
      surface.protocol_category_composition_summary
              .invalid_protocol_composition_sites ==
          surface.invalid_protocol_composition_sites_total &&
      surface.protocol_category_composition_summary
              .invalid_protocol_composition_sites <=
          surface.protocol_category_composition_summary
              .total_composition_sites();
  record.class_protocol_category_linking_ready =
      deterministic_class_protocol_category_linking_handoff &&
      surface.class_protocol_category_linking_summary.declared_interfaces ==
          surface.interface_implementation_summary.declared_interfaces &&
      surface.class_protocol_category_linking_summary.resolved_interfaces ==
          surface.interface_implementation_summary.resolved_interfaces &&
      surface.class_protocol_category_linking_summary.declared_implementations ==
          surface.interface_implementation_summary.declared_implementations &&
      surface.class_protocol_category_linking_summary.resolved_implementations ==
          surface.interface_implementation_summary.resolved_implementations &&
      surface.class_protocol_category_linking_summary.interface_method_symbols ==
          surface.interface_method_symbols_total &&
      surface.class_protocol_category_linking_summary
              .implementation_method_symbols ==
          surface.implementation_method_symbols_total &&
      surface.class_protocol_category_linking_summary
              .linked_implementation_symbols ==
          surface.linked_implementation_symbols_total &&
      surface.class_protocol_category_linking_summary
              .protocol_composition_sites ==
          surface.protocol_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .protocol_composition_symbols ==
          surface.protocol_composition_symbols_total &&
      surface.class_protocol_category_linking_summary
              .category_composition_sites ==
          surface.category_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .category_composition_symbols ==
          surface.category_composition_symbols_total &&
      surface.class_protocol_category_linking_summary
              .invalid_protocol_composition_sites ==
          surface.invalid_protocol_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .invalid_protocol_composition_sites <=
          surface.class_protocol_category_linking_summary
              .total_composition_sites() &&
      surface.class_protocol_category_linking_summary.deterministic;
  record.selector_normalization_ready =
      deterministic_selector_normalization_handoff &&
      surface.selector_normalization_summary.methods_total ==
          surface.selector_normalization_methods_total &&
      surface.selector_normalization_summary.normalized_methods ==
          surface.selector_normalization_normalized_methods_total &&
      surface.selector_normalization_summary.selector_piece_entries ==
          surface.selector_normalization_piece_entries_total &&
      surface.selector_normalization_summary
              .selector_parameter_piece_entries ==
          surface.selector_normalization_parameter_piece_entries_total &&
      surface.selector_normalization_summary.selector_pieceless_methods ==
          surface.selector_normalization_pieceless_methods_total &&
      surface.selector_normalization_summary.selector_spelling_mismatches ==
          surface.selector_normalization_spelling_mismatches_total &&
      surface.selector_normalization_summary.selector_arity_mismatches ==
          surface.selector_normalization_arity_mismatches_total &&
      surface.selector_normalization_summary
              .selector_parameter_linkage_mismatches ==
          surface.selector_normalization_parameter_linkage_mismatches_total &&
      surface.selector_normalization_summary
              .selector_normalization_flag_mismatches ==
          surface.selector_normalization_flag_mismatches_total &&
      surface.selector_normalization_summary.selector_missing_keyword_pieces ==
          surface.selector_normalization_missing_keyword_pieces_total &&
      surface.selector_normalization_summary.normalized_methods <=
          surface.selector_normalization_summary.methods_total &&
      surface.selector_normalization_summary
              .selector_parameter_piece_entries <=
          surface.selector_normalization_summary.selector_piece_entries &&
      surface.selector_normalization_summary.contract_violations() <=
          surface.selector_normalization_summary.methods_total &&
      surface.selector_normalization_summary.deterministic;
  record.property_attribute_ready =
      deterministic_property_attribute_handoff &&
      surface.property_attribute_summary.properties_total ==
          surface.property_attribute_properties_total &&
      surface.property_attribute_summary.attribute_entries ==
          surface.property_attribute_entries_total &&
      surface.property_attribute_summary.readonly_modifiers ==
          surface.property_attribute_readonly_modifiers_total &&
      surface.property_attribute_summary.readwrite_modifiers ==
          surface.property_attribute_readwrite_modifiers_total &&
      surface.property_attribute_summary.atomic_modifiers ==
          surface.property_attribute_atomic_modifiers_total &&
      surface.property_attribute_summary.nonatomic_modifiers ==
          surface.property_attribute_nonatomic_modifiers_total &&
      surface.property_attribute_summary.copy_modifiers ==
          surface.property_attribute_copy_modifiers_total &&
      surface.property_attribute_summary.strong_modifiers ==
          surface.property_attribute_strong_modifiers_total &&
      surface.property_attribute_summary.weak_modifiers ==
          surface.property_attribute_weak_modifiers_total &&
      surface.property_attribute_summary.assign_modifiers ==
          surface.property_attribute_assign_modifiers_total &&
      surface.property_attribute_summary.getter_modifiers ==
          surface.property_attribute_getter_modifiers_total &&
      surface.property_attribute_summary.setter_modifiers ==
          surface.property_attribute_setter_modifiers_total &&
      surface.property_attribute_summary.invalid_attribute_entries ==
          surface.property_attribute_invalid_attribute_entries_total &&
      surface.property_attribute_summary.property_contract_violations ==
          surface.property_attribute_contract_violations_total &&
      surface.property_attribute_summary.getter_modifiers <=
          surface.property_attribute_summary.properties_total &&
      surface.property_attribute_summary.setter_modifiers <=
          surface.property_attribute_summary.properties_total &&
      surface.property_attribute_summary.deterministic;
  record.type_annotation_surface_ready =
      deterministic_type_annotation_surface_handoff &&
      surface.type_annotation_surface_summary.generic_suffix_sites ==
          surface.type_annotation_generic_suffix_sites_total &&
      surface.type_annotation_surface_summary.pointer_declarator_sites ==
          surface.type_annotation_pointer_declarator_sites_total &&
      surface.type_annotation_surface_summary.nullability_suffix_sites ==
          surface.type_annotation_nullability_suffix_sites_total &&
      surface.type_annotation_surface_summary.ownership_qualifier_sites ==
          surface.type_annotation_ownership_qualifier_sites_total &&
      surface.type_annotation_surface_summary.object_pointer_type_sites ==
          surface.type_annotation_object_pointer_type_sites_total &&
      surface.type_annotation_surface_summary.invalid_generic_suffix_sites ==
          surface.type_annotation_invalid_generic_suffix_sites_total &&
      surface.type_annotation_surface_summary
              .invalid_pointer_declarator_sites ==
          surface.type_annotation_invalid_pointer_declarator_sites_total &&
      surface.type_annotation_surface_summary
              .invalid_nullability_suffix_sites ==
          surface.type_annotation_invalid_nullability_suffix_sites_total &&
      surface.type_annotation_surface_summary
              .invalid_ownership_qualifier_sites ==
          surface.type_annotation_invalid_ownership_qualifier_sites_total &&
      surface.type_annotation_surface_summary.invalid_generic_suffix_sites <=
          surface.type_annotation_surface_summary.generic_suffix_sites &&
      surface.type_annotation_surface_summary
              .invalid_pointer_declarator_sites <=
          surface.type_annotation_surface_summary.pointer_declarator_sites &&
      surface.type_annotation_surface_summary
              .invalid_nullability_suffix_sites <=
          surface.type_annotation_surface_summary.nullability_suffix_sites &&
      surface.type_annotation_surface_summary
              .invalid_ownership_qualifier_sites <=
          surface.type_annotation_surface_summary.ownership_qualifier_sites &&
      surface.type_annotation_surface_summary.invalid_type_annotation_sites() <=
          surface.type_annotation_surface_summary.total_type_annotation_sites() &&
      surface.type_annotation_surface_summary.deterministic;
  record.lightweight_generic_constraint_ready =
      deterministic_lightweight_generic_constraint_handoff &&
      surface.lightweight_generic_constraint_summary.generic_constraint_sites ==
          surface.lightweight_generic_constraint_sites_total &&
      surface.lightweight_generic_constraint_summary.generic_suffix_sites ==
          surface.lightweight_generic_constraint_generic_suffix_sites_total &&
      surface.lightweight_generic_constraint_summary.object_pointer_type_sites ==
          surface
              .lightweight_generic_constraint_object_pointer_type_sites_total &&
      surface.lightweight_generic_constraint_summary
              .terminated_generic_suffix_sites ==
          surface
              .lightweight_generic_constraint_terminated_generic_suffix_sites_total &&
      surface.lightweight_generic_constraint_summary.pointer_declarator_sites ==
          surface.lightweight_generic_constraint_pointer_declarator_sites_total &&
      surface.lightweight_generic_constraint_summary.normalized_constraint_sites ==
          surface.lightweight_generic_constraint_normalized_sites_total &&
      surface.lightweight_generic_constraint_summary.contract_violation_sites ==
          surface
              .lightweight_generic_constraint_contract_violation_sites_total &&
      surface.lightweight_generic_constraint_summary
              .terminated_generic_suffix_sites <=
          surface.lightweight_generic_constraint_summary
              .generic_suffix_sites &&
      surface.lightweight_generic_constraint_summary.normalized_constraint_sites <=
          surface.lightweight_generic_constraint_summary
              .generic_constraint_sites &&
      surface.lightweight_generic_constraint_summary.contract_violation_sites <=
          surface.lightweight_generic_constraint_summary
              .generic_constraint_sites &&
      surface.lightweight_generic_constraint_summary.deterministic;
  record.nullability_flow_warning_precision_ready =
      deterministic_nullability_flow_warning_precision_handoff &&
      surface.nullability_flow_warning_precision_summary.nullability_flow_sites ==
          surface.nullability_flow_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .object_pointer_type_sites ==
          surface.nullability_flow_object_pointer_type_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .nullability_suffix_sites ==
          surface.nullability_flow_nullability_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .nullable_suffix_sites ==
          surface.nullability_flow_nullable_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary.nonnull_suffix_sites ==
          surface.nullability_flow_nonnull_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary.normalized_sites ==
          surface.nullability_flow_normalized_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .contract_violation_sites ==
          surface.nullability_flow_contract_violation_sites_total &&
      surface.nullability_flow_warning_precision_summary.normalized_sites <=
          surface.nullability_flow_warning_precision_summary
              .nullability_flow_sites &&
      surface.nullability_flow_warning_precision_summary.contract_violation_sites <=
          surface.nullability_flow_warning_precision_summary
              .nullability_flow_sites &&
      surface.nullability_flow_warning_precision_summary
              .nullability_suffix_sites ==
          surface.nullability_flow_warning_precision_summary
              .nullable_suffix_sites +
              surface.nullability_flow_warning_precision_summary
                  .nonnull_suffix_sites &&
      surface.nullability_flow_warning_precision_summary.deterministic;
  record.protocol_qualified_object_type_ready =
      deterministic_protocol_qualified_object_type_handoff &&
      surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites ==
          surface.protocol_qualified_object_type_sites_total &&
      surface.protocol_qualified_object_type_summary
              .protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.object_pointer_type_sites ==
          surface
              .protocol_qualified_object_type_object_pointer_type_sites_total &&
      surface.protocol_qualified_object_type_summary
              .terminated_protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_terminated_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.pointer_declarator_sites ==
          surface
              .protocol_qualified_object_type_pointer_declarator_sites_total &&
      surface.protocol_qualified_object_type_summary
              .normalized_protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_normalized_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.contract_violation_sites ==
          surface
              .protocol_qualified_object_type_contract_violation_sites_total &&
      surface.protocol_qualified_object_type_summary
              .terminated_protocol_composition_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_composition_sites &&
      surface.protocol_qualified_object_type_summary
              .normalized_protocol_composition_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites &&
      surface.protocol_qualified_object_type_summary.contract_violation_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites &&
      surface.protocol_qualified_object_type_summary.deterministic;
  record.variance_bridge_cast_ready =
      deterministic_variance_bridge_cast_handoff &&
      surface.variance_bridge_cast_summary.variance_bridge_cast_sites ==
          surface.variance_bridge_cast_sites_total &&
      surface.variance_bridge_cast_summary.protocol_composition_sites ==
          surface.variance_bridge_cast_protocol_composition_sites_total &&
      surface.variance_bridge_cast_summary.ownership_qualifier_sites ==
          surface.variance_bridge_cast_ownership_qualifier_sites_total &&
      surface.variance_bridge_cast_summary.object_pointer_type_sites ==
          surface.variance_bridge_cast_object_pointer_type_sites_total &&
      surface.variance_bridge_cast_summary.pointer_declarator_sites ==
          surface.variance_bridge_cast_pointer_declarator_sites_total &&
      surface.variance_bridge_cast_summary.normalized_sites ==
          surface.variance_bridge_cast_normalized_sites_total &&
      surface.variance_bridge_cast_summary.contract_violation_sites ==
          surface.variance_bridge_cast_contract_violation_sites_total &&
      surface.variance_bridge_cast_summary.protocol_composition_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.normalized_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.contract_violation_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.deterministic;
  record.passed_publication_count =
      Objc3SemaEvidenceCount(record.semantic_diagnostics_ready) +
      Objc3SemaEvidenceCount(record.type_metadata_handoff_ready) +
      Objc3SemaEvidenceCount(record.interface_implementation_ready) +
      Objc3SemaEvidenceCount(record.protocol_category_composition_ready) +
      Objc3SemaEvidenceCount(record.class_protocol_category_linking_ready) +
      Objc3SemaEvidenceCount(record.selector_normalization_ready) +
      Objc3SemaEvidenceCount(record.property_attribute_ready) +
      Objc3SemaEvidenceCount(record.type_annotation_surface_ready) +
      Objc3SemaEvidenceCount(record.lightweight_generic_constraint_ready) +
      Objc3SemaEvidenceCount(
          record.nullability_flow_warning_precision_ready) +
      Objc3SemaEvidenceCount(record.protocol_qualified_object_type_ready) +
      Objc3SemaEvidenceCount(record.variance_bridge_cast_ready);
  record.failed_publication_count =
      record.required_publication_count >= record.passed_publication_count
          ? (record.required_publication_count -
             record.passed_publication_count)
          : record.required_publication_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.core_semantic_parity_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_publication_count == 12u &&
      record.passed_publication_count == record.required_publication_count &&
      record.failed_publication_count == 0u;
  return record;
}

inline Objc3SemaModuleSemanticParityPublicationReadinessRecord
BuildObjc3SemaModuleSemanticParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_generic_metadata_abi_handoff,
    bool deterministic_module_import_graph_handoff,
    bool deterministic_namespace_collision_shadowing_handoff,
    bool deterministic_public_private_api_partition_handoff,
    bool deterministic_incremental_module_cache_invalidation_handoff) {
  Objc3SemaModuleSemanticParityPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.generic_metadata_abi_ready =
      deterministic_generic_metadata_abi_handoff &&
      surface.generic_metadata_abi_summary.generic_metadata_abi_sites ==
          surface.generic_metadata_abi_sites_total &&
      surface.generic_metadata_abi_summary.generic_suffix_sites ==
          surface.generic_metadata_abi_generic_suffix_sites_total &&
      surface.generic_metadata_abi_summary.protocol_composition_sites ==
          surface.generic_metadata_abi_protocol_composition_sites_total &&
      surface.generic_metadata_abi_summary.ownership_qualifier_sites ==
          surface.generic_metadata_abi_ownership_qualifier_sites_total &&
      surface.generic_metadata_abi_summary.object_pointer_type_sites ==
          surface.generic_metadata_abi_object_pointer_type_sites_total &&
      surface.generic_metadata_abi_summary.pointer_declarator_sites ==
          surface.generic_metadata_abi_pointer_declarator_sites_total &&
      surface.generic_metadata_abi_summary.normalized_sites ==
          surface.generic_metadata_abi_normalized_sites_total &&
      surface.generic_metadata_abi_summary.contract_violation_sites ==
          surface.generic_metadata_abi_contract_violation_sites_total &&
      surface.generic_metadata_abi_summary.generic_suffix_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.protocol_composition_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.normalized_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.contract_violation_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.deterministic;
  record.module_import_graph_ready =
      deterministic_module_import_graph_handoff &&
      surface.module_import_graph_summary.module_import_graph_sites ==
          surface.module_import_graph_sites_total &&
      surface.module_import_graph_summary.import_edge_candidate_sites ==
          surface.module_import_graph_import_edge_candidate_sites_total &&
      surface.module_import_graph_summary.namespace_segment_sites ==
          surface.module_import_graph_namespace_segment_sites_total &&
      surface.module_import_graph_summary.object_pointer_type_sites ==
          surface.module_import_graph_object_pointer_type_sites_total &&
      surface.module_import_graph_summary.pointer_declarator_sites ==
          surface.module_import_graph_pointer_declarator_sites_total &&
      surface.module_import_graph_summary.normalized_sites ==
          surface.module_import_graph_normalized_sites_total &&
      surface.module_import_graph_summary.contract_violation_sites ==
          surface.module_import_graph_contract_violation_sites_total &&
      surface.module_import_graph_summary.import_edge_candidate_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.namespace_segment_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.normalized_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.contract_violation_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.deterministic;
  record.namespace_collision_shadowing_ready =
      deterministic_namespace_collision_shadowing_handoff &&
      surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites ==
          surface.namespace_collision_shadowing_sites_total &&
      surface.namespace_collision_shadowing_summary.namespace_segment_sites ==
          surface
              .namespace_collision_shadowing_namespace_segment_sites_total &&
      surface.namespace_collision_shadowing_summary
              .import_edge_candidate_sites ==
          surface
              .namespace_collision_shadowing_import_edge_candidate_sites_total &&
      surface.namespace_collision_shadowing_summary.object_pointer_type_sites ==
          surface
              .namespace_collision_shadowing_object_pointer_type_sites_total &&
      surface.namespace_collision_shadowing_summary.pointer_declarator_sites ==
          surface
              .namespace_collision_shadowing_pointer_declarator_sites_total &&
      surface.namespace_collision_shadowing_summary.normalized_sites ==
          surface.namespace_collision_shadowing_normalized_sites_total &&
      surface.namespace_collision_shadowing_summary.contract_violation_sites ==
          surface
              .namespace_collision_shadowing_contract_violation_sites_total &&
      surface.namespace_collision_shadowing_summary.namespace_segment_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary
              .import_edge_candidate_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary.normalized_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary.contract_violation_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary.deterministic;
  record.public_private_api_partition_ready =
      deterministic_public_private_api_partition_handoff &&
      surface.public_private_api_partition_summary
              .public_private_api_partition_sites ==
          surface.public_private_api_partition_sites_total &&
      surface.public_private_api_partition_summary.namespace_segment_sites ==
          surface.public_private_api_partition_namespace_segment_sites_total &&
      surface.public_private_api_partition_summary
              .import_edge_candidate_sites ==
          surface.public_private_api_partition_import_edge_candidate_sites_total &&
      surface.public_private_api_partition_summary.object_pointer_type_sites ==
          surface.public_private_api_partition_object_pointer_type_sites_total &&
      surface.public_private_api_partition_summary.pointer_declarator_sites ==
          surface.public_private_api_partition_pointer_declarator_sites_total &&
      surface.public_private_api_partition_summary.normalized_sites ==
          surface.public_private_api_partition_normalized_sites_total &&
      surface.public_private_api_partition_summary.contract_violation_sites ==
          surface.public_private_api_partition_contract_violation_sites_total &&
      surface.public_private_api_partition_summary.namespace_segment_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.import_edge_candidate_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.normalized_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.contract_violation_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.deterministic;
  record.incremental_module_cache_invalidation_ready =
      deterministic_incremental_module_cache_invalidation_handoff &&
      surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites ==
          surface.incremental_module_cache_invalidation_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .namespace_segment_sites ==
          surface
              .incremental_module_cache_invalidation_namespace_segment_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .import_edge_candidate_sites ==
          surface
              .incremental_module_cache_invalidation_import_edge_candidate_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .object_pointer_type_sites ==
          surface
              .incremental_module_cache_invalidation_object_pointer_type_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .pointer_declarator_sites ==
          surface
              .incremental_module_cache_invalidation_pointer_declarator_sites_total &&
      surface.incremental_module_cache_invalidation_summary.normalized_sites ==
          surface.incremental_module_cache_invalidation_normalized_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .cache_invalidation_candidate_sites ==
          surface
              .incremental_module_cache_invalidation_cache_invalidation_candidate_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .contract_violation_sites ==
          surface
              .incremental_module_cache_invalidation_contract_violation_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .namespace_segment_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary
              .import_edge_candidate_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary.normalized_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary
              .cache_invalidation_candidate_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary
              .contract_violation_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary.deterministic;
  record.passed_publication_count =
      Objc3SemaEvidenceCount(record.generic_metadata_abi_ready) +
      Objc3SemaEvidenceCount(record.module_import_graph_ready) +
      Objc3SemaEvidenceCount(record.namespace_collision_shadowing_ready) +
      Objc3SemaEvidenceCount(record.public_private_api_partition_ready) +
      Objc3SemaEvidenceCount(
          record.incremental_module_cache_invalidation_ready);
  record.failed_publication_count =
      record.required_publication_count >= record.passed_publication_count
          ? (record.required_publication_count -
             record.passed_publication_count)
          : record.required_publication_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.module_semantic_parity_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_publication_count == 5u &&
      record.passed_publication_count == record.required_publication_count &&
      record.failed_publication_count == 0u;
  return record;
}

Objc3SemaIntermoduleFlowSummaryReadinessRecord
BuildObjc3SemaIntermoduleFlowSummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_cross_module_conformance_handoff,
    bool deterministic_throws_propagation_handoff);

Objc3SemaIntermoduleFlowParityPublicationReadinessRecord
BuildObjc3SemaIntermoduleFlowParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_cross_module_conformance_handoff,
    bool deterministic_throws_propagation_handoff);

inline Objc3SemaConcurrencyParityPublicationReadinessRecord
BuildObjc3SemaConcurrencyParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_actor_isolation_sendability_handoff,
    bool deterministic_task_runtime_cancellation_handoff,
    bool deterministic_concurrency_replay_race_guard_handoff) {
  Objc3SemaConcurrencyParityPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.actor_isolation_sendability_ready =
      deterministic_actor_isolation_sendability_handoff &&
      surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites ==
          surface.actor_isolation_sendability_sites_total &&
      surface.actor_isolation_sendability_summary.actor_isolation_decl_sites ==
          surface.actor_isolation_decl_sites_total &&
      surface.actor_isolation_sendability_summary.actor_hop_sites ==
          surface.actor_hop_sites_total &&
      surface.actor_isolation_sendability_summary.sendable_annotation_sites ==
          surface.sendable_annotation_sites_total &&
      surface.actor_isolation_sendability_summary.non_sendable_crossing_sites ==
          surface.non_sendable_crossing_sites_total &&
      surface.actor_isolation_sendability_summary.isolation_boundary_sites ==
          surface.actor_isolation_sendability_isolation_boundary_sites_total &&
      surface.actor_isolation_sendability_summary.normalized_sites ==
          surface.actor_isolation_sendability_normalized_sites_total &&
      surface.actor_isolation_sendability_summary.gate_blocked_sites ==
          surface.actor_isolation_sendability_gate_blocked_sites_total &&
      surface.actor_isolation_sendability_summary.contract_violation_sites ==
          surface.actor_isolation_sendability_contract_violation_sites_total &&
      surface.actor_isolation_sendability_summary.actor_isolation_decl_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.actor_hop_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.sendable_annotation_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.non_sendable_crossing_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.isolation_boundary_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.normalized_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.gate_blocked_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.gate_blocked_sites <=
          surface.actor_isolation_sendability_summary
              .non_sendable_crossing_sites &&
      surface.actor_isolation_sendability_summary.contract_violation_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.normalized_sites +
              surface.actor_isolation_sendability_summary.gate_blocked_sites ==
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.deterministic;
  record.task_runtime_cancellation_ready =
      deterministic_task_runtime_cancellation_handoff &&
      surface.task_runtime_cancellation_summary.task_runtime_interop_sites ==
          surface.task_runtime_cancellation_sites_total &&
      surface.task_runtime_cancellation_summary.runtime_hook_sites ==
          surface.task_runtime_cancellation_runtime_hook_sites_total &&
      surface.task_runtime_cancellation_summary.cancellation_check_sites ==
          surface.task_runtime_cancellation_cancellation_check_sites_total &&
      surface.task_runtime_cancellation_summary.cancellation_handler_sites ==
          surface
              .task_runtime_cancellation_cancellation_handler_sites_total &&
      surface.task_runtime_cancellation_summary.suspension_point_sites ==
          surface.task_runtime_cancellation_suspension_point_sites_total &&
      surface.task_runtime_cancellation_summary
              .cancellation_propagation_sites ==
          surface
              .task_runtime_cancellation_cancellation_propagation_sites_total &&
      surface.task_runtime_cancellation_summary.normalized_sites ==
          surface.task_runtime_cancellation_normalized_sites_total &&
      surface.task_runtime_cancellation_summary.gate_blocked_sites ==
          surface.task_runtime_cancellation_gate_blocked_sites_total &&
      surface.task_runtime_cancellation_summary.contract_violation_sites ==
          surface.task_runtime_cancellation_contract_violation_sites_total &&
      surface.task_runtime_cancellation_summary.runtime_hook_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.cancellation_check_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.cancellation_handler_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.suspension_point_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.cancellation_propagation_sites <=
          surface.task_runtime_cancellation_summary.cancellation_check_sites &&
      surface.task_runtime_cancellation_summary.cancellation_propagation_sites <=
          surface.task_runtime_cancellation_summary.cancellation_handler_sites &&
      surface.task_runtime_cancellation_summary.normalized_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.gate_blocked_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.gate_blocked_sites <=
          surface.task_runtime_cancellation_summary
              .cancellation_propagation_sites &&
      surface.task_runtime_cancellation_summary.contract_violation_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.normalized_sites +
              surface.task_runtime_cancellation_summary.gate_blocked_sites ==
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.deterministic;
  record.concurrency_replay_race_guard_ready =
      deterministic_concurrency_replay_race_guard_handoff &&
      surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites ==
          surface.concurrency_replay_race_guard_sites_total &&
      surface.concurrency_replay_race_guard_summary.concurrency_replay_sites ==
          surface
              .concurrency_replay_race_guard_concurrency_replay_sites_total &&
      surface.concurrency_replay_race_guard_summary.replay_proof_sites ==
          surface.concurrency_replay_race_guard_replay_proof_sites_total &&
      surface.concurrency_replay_race_guard_summary.race_guard_sites ==
          surface.concurrency_replay_race_guard_race_guard_sites_total &&
      surface.concurrency_replay_race_guard_summary.task_handoff_sites ==
          surface.concurrency_replay_race_guard_task_handoff_sites_total &&
      surface.concurrency_replay_race_guard_summary.actor_isolation_sites ==
          surface.concurrency_replay_race_guard_actor_isolation_sites_total &&
      surface.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites ==
          surface
              .concurrency_replay_race_guard_deterministic_schedule_sites_total &&
      surface.concurrency_replay_race_guard_summary.guard_blocked_sites ==
          surface.concurrency_replay_race_guard_guard_blocked_sites_total &&
      surface.concurrency_replay_race_guard_summary.contract_violation_sites ==
          surface.concurrency_replay_race_guard_contract_violation_sites_total &&
      surface.concurrency_replay_race_guard_summary.concurrency_replay_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary.replay_proof_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary.race_guard_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary.task_handoff_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary.actor_isolation_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      surface.concurrency_replay_race_guard_summary.guard_blocked_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      surface.concurrency_replay_race_guard_summary.contract_violation_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites +
              surface.concurrency_replay_race_guard_summary
                  .guard_blocked_sites ==
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      surface.concurrency_replay_race_guard_summary.deterministic;
  record.passed_publication_count =
      Objc3SemaEvidenceCount(record.actor_isolation_sendability_ready) +
      Objc3SemaEvidenceCount(record.task_runtime_cancellation_ready) +
      Objc3SemaEvidenceCount(record.concurrency_replay_race_guard_ready);
  record.failed_publication_count =
      record.required_publication_count >= record.passed_publication_count
          ? (record.required_publication_count -
             record.passed_publication_count)
          : record.required_publication_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.concurrency_parity_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_publication_count == 3u &&
      record.passed_publication_count == record.required_publication_count &&
      record.failed_publication_count == 0u;
  return record;
}

inline Objc3SemaUnsafeErrorParityValidationReadinessRecord
BuildObjc3SemaUnsafeErrorParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_unsafe_pointer_extension_handoff,
    bool deterministic_inline_asm_intrinsic_governance_handoff,
    bool deterministic_ns_error_bridging_handoff,
    bool deterministic_error_diagnostics_recovery_handoff,
    bool deterministic_result_like_lowering_handoff) {
  Objc3SemaUnsafeErrorParityValidationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.unsafe_pointer_extension_ready =
      deterministic_unsafe_pointer_extension_handoff &&
      surface.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites ==
          surface.unsafe_pointer_extension_sites_total &&
      surface.unsafe_pointer_extension_summary.unsafe_keyword_sites ==
          surface.unsafe_pointer_extension_unsafe_keyword_sites_total &&
      surface.unsafe_pointer_extension_summary.pointer_arithmetic_sites ==
          surface.unsafe_pointer_extension_pointer_arithmetic_sites_total &&
      surface.unsafe_pointer_extension_summary.raw_pointer_type_sites ==
          surface.unsafe_pointer_extension_raw_pointer_type_sites_total &&
      surface.unsafe_pointer_extension_summary.unsafe_operation_sites ==
          surface.unsafe_pointer_extension_unsafe_operation_sites_total &&
      surface.unsafe_pointer_extension_summary.normalized_sites ==
          surface.unsafe_pointer_extension_normalized_sites_total &&
      surface.unsafe_pointer_extension_summary.gate_blocked_sites ==
          surface.unsafe_pointer_extension_gate_blocked_sites_total &&
      surface.unsafe_pointer_extension_summary.contract_violation_sites ==
          surface.unsafe_pointer_extension_contract_violation_sites_total &&
      surface.unsafe_pointer_extension_summary.unsafe_keyword_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.pointer_arithmetic_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.raw_pointer_type_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.unsafe_operation_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.normalized_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.gate_blocked_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.contract_violation_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.normalized_sites +
              surface.unsafe_pointer_extension_summary.gate_blocked_sites ==
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.deterministic;
  record.inline_asm_intrinsic_governance_ready =
      deterministic_inline_asm_intrinsic_governance_handoff &&
      surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites ==
          surface.inline_asm_intrinsic_governance_sites_total &&
      surface.inline_asm_intrinsic_governance_summary.inline_asm_sites ==
          surface.inline_asm_intrinsic_governance_inline_asm_sites_total &&
      surface.inline_asm_intrinsic_governance_summary.intrinsic_sites ==
          surface.inline_asm_intrinsic_governance_intrinsic_sites_total &&
      surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites ==
          surface
              .inline_asm_intrinsic_governance_governed_intrinsic_sites_total &&
      surface.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites ==
          surface
              .inline_asm_intrinsic_governance_privileged_intrinsic_sites_total &&
      surface.inline_asm_intrinsic_governance_summary.normalized_sites ==
          surface.inline_asm_intrinsic_governance_normalized_sites_total &&
      surface.inline_asm_intrinsic_governance_summary.gate_blocked_sites ==
          surface.inline_asm_intrinsic_governance_gate_blocked_sites_total &&
      surface.inline_asm_intrinsic_governance_summary
              .contract_violation_sites ==
          surface
              .inline_asm_intrinsic_governance_contract_violation_sites_total &&
      surface.inline_asm_intrinsic_governance_summary.inline_asm_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.intrinsic_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites <=
          surface.inline_asm_intrinsic_governance_summary.intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.normalized_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.gate_blocked_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary
              .contract_violation_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.inline_asm_sites ==
          surface.throws_propagation_summary.cache_invalidation_candidate_sites &&
      surface.inline_asm_intrinsic_governance_summary.intrinsic_sites ==
          surface.unsafe_pointer_extension_summary.unsafe_operation_sites &&
      surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites <=
          surface.throws_propagation_summary.normalized_sites &&
      surface.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites <=
          surface.unsafe_pointer_extension_summary.normalized_sites &&
      surface.inline_asm_intrinsic_governance_summary.normalized_sites >=
          surface.inline_asm_intrinsic_governance_summary.inline_asm_sites &&
      surface.inline_asm_intrinsic_governance_summary.normalized_sites -
              surface.inline_asm_intrinsic_governance_summary
                  .inline_asm_sites ==
          surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.gate_blocked_sites ==
          surface.inline_asm_intrinsic_governance_summary.intrinsic_sites -
              surface.inline_asm_intrinsic_governance_summary
                  .governed_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.normalized_sites +
              surface.inline_asm_intrinsic_governance_summary
                  .gate_blocked_sites ==
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.deterministic;
  record.ns_error_bridging_ready =
      deterministic_ns_error_bridging_handoff &&
      surface.ns_error_bridging_summary.ns_error_bridging_sites ==
          surface.ns_error_bridging_sites_total &&
      surface.ns_error_bridging_summary.ns_error_parameter_sites ==
          surface.ns_error_bridging_ns_error_parameter_sites_total &&
      surface.ns_error_bridging_summary.ns_error_out_parameter_sites ==
          surface.ns_error_bridging_ns_error_out_parameter_sites_total &&
      surface.ns_error_bridging_summary.ns_error_bridge_path_sites ==
          surface.ns_error_bridging_ns_error_bridge_path_sites_total &&
      surface.ns_error_bridging_summary.failable_call_sites ==
          surface.ns_error_bridging_failable_call_sites_total &&
      surface.ns_error_bridging_summary.normalized_sites ==
          surface.ns_error_bridging_normalized_sites_total &&
      surface.ns_error_bridging_summary.bridge_boundary_sites ==
          surface.ns_error_bridging_bridge_boundary_sites_total &&
      surface.ns_error_bridging_summary.contract_violation_sites ==
          surface.ns_error_bridging_contract_violation_sites_total &&
      surface.ns_error_bridging_summary.ns_error_parameter_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.ns_error_out_parameter_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.ns_error_bridge_path_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.failable_call_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.normalized_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.bridge_boundary_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.normalized_sites +
              surface.ns_error_bridging_summary.bridge_boundary_sites ==
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.contract_violation_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.deterministic;
  record.error_diagnostics_recovery_ready =
      deterministic_error_diagnostics_recovery_handoff &&
      surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites ==
          surface.error_diagnostics_recovery_sites_total &&
      surface.error_diagnostics_recovery_summary.diagnostic_emit_sites ==
          surface.error_diagnostics_recovery_diagnostic_emit_sites_total &&
      surface.error_diagnostics_recovery_summary.recovery_anchor_sites ==
          surface.error_diagnostics_recovery_recovery_anchor_sites_total &&
      surface.error_diagnostics_recovery_summary.recovery_boundary_sites ==
          surface.error_diagnostics_recovery_recovery_boundary_sites_total &&
      surface.error_diagnostics_recovery_summary.fail_closed_diagnostic_sites ==
          surface
              .error_diagnostics_recovery_fail_closed_diagnostic_sites_total &&
      surface.error_diagnostics_recovery_summary.normalized_sites ==
          surface.error_diagnostics_recovery_normalized_sites_total &&
      surface.error_diagnostics_recovery_summary.gate_blocked_sites ==
          surface.error_diagnostics_recovery_gate_blocked_sites_total &&
      surface.error_diagnostics_recovery_summary.contract_violation_sites ==
          surface.error_diagnostics_recovery_contract_violation_sites_total &&
      surface.error_diagnostics_recovery_summary.diagnostic_emit_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.recovery_anchor_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.recovery_boundary_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.fail_closed_diagnostic_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.fail_closed_diagnostic_sites <=
          surface.error_diagnostics_recovery_summary.diagnostic_emit_sites &&
      surface.error_diagnostics_recovery_summary.normalized_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.gate_blocked_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.contract_violation_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.normalized_sites +
              surface.error_diagnostics_recovery_summary.gate_blocked_sites ==
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.deterministic;
  record.result_like_lowering_ready =
      deterministic_result_like_lowering_handoff &&
      surface.result_like_lowering_summary.result_like_sites ==
          surface.result_like_lowering_sites_total &&
      surface.result_like_lowering_summary.result_success_sites ==
          surface.result_like_lowering_result_success_sites_total &&
      surface.result_like_lowering_summary.result_failure_sites ==
          surface.result_like_lowering_result_failure_sites_total &&
      surface.result_like_lowering_summary.result_branch_sites ==
          surface.result_like_lowering_result_branch_sites_total &&
      surface.result_like_lowering_summary.result_payload_sites ==
          surface.result_like_lowering_result_payload_sites_total &&
      surface.result_like_lowering_summary.normalized_sites ==
          surface.result_like_lowering_normalized_sites_total &&
      surface.result_like_lowering_summary.branch_merge_sites ==
          surface.result_like_lowering_branch_merge_sites_total &&
      surface.result_like_lowering_summary.contract_violation_sites ==
          surface.result_like_lowering_contract_violation_sites_total &&
      surface.result_like_lowering_summary.result_success_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.result_failure_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.result_branch_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.result_payload_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.normalized_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.branch_merge_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.contract_violation_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.result_success_sites +
              surface.result_like_lowering_summary.result_failure_sites ==
          surface.result_like_lowering_summary.normalized_sites &&
      surface.result_like_lowering_summary.normalized_sites +
              surface.result_like_lowering_summary.branch_merge_sites ==
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.deterministic;
  record.passed_validation_count =
      Objc3SemaEvidenceCount(record.unsafe_pointer_extension_ready) +
      Objc3SemaEvidenceCount(record.inline_asm_intrinsic_governance_ready) +
      Objc3SemaEvidenceCount(record.ns_error_bridging_ready) +
      Objc3SemaEvidenceCount(record.error_diagnostics_recovery_ready) +
      Objc3SemaEvidenceCount(record.result_like_lowering_ready);
  record.failed_validation_count =
      record.required_validation_count >= record.passed_validation_count
          ? (record.required_validation_count -
             record.passed_validation_count)
          : record.required_validation_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.unsafe_error_parity_validation_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_validation_count == 5u &&
      record.passed_validation_count == record.required_validation_count &&
      record.failed_validation_count == 0u;
  return record;
}

inline Objc3SemaControlBindingParityValidationReadinessRecord
BuildObjc3SemaControlBindingParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_unwind_cleanup_handoff,
    bool deterministic_async_continuation_handoff,
    bool deterministic_symbol_graph_scope_resolution_handoff,
    bool deterministic_method_lookup_override_conflict_handoff,
    bool deterministic_property_synthesis_ivar_binding_handoff,
    bool deterministic_id_class_sel_object_pointer_type_checking_handoff) {
  Objc3SemaControlBindingParityValidationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.unwind_cleanup_ready =
      deterministic_unwind_cleanup_handoff &&
      surface.unwind_cleanup_summary.unwind_cleanup_sites ==
          surface.unwind_cleanup_sites_total &&
      surface.unwind_cleanup_summary.exceptional_exit_sites ==
          surface.unwind_cleanup_exceptional_exit_sites_total &&
      surface.unwind_cleanup_summary.cleanup_action_sites ==
          surface.unwind_cleanup_action_sites_total &&
      surface.unwind_cleanup_summary.cleanup_scope_sites ==
          surface.unwind_cleanup_scope_sites_total &&
      surface.unwind_cleanup_summary.cleanup_resume_sites ==
          surface.unwind_cleanup_resume_sites_total &&
      surface.unwind_cleanup_summary.normalized_sites ==
          surface.unwind_cleanup_normalized_sites_total &&
      surface.unwind_cleanup_summary.fail_closed_sites ==
          surface.unwind_cleanup_fail_closed_sites_total &&
      surface.unwind_cleanup_summary.contract_violation_sites ==
          surface.unwind_cleanup_contract_violation_sites_total &&
      surface.unwind_cleanup_summary.exceptional_exit_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.cleanup_action_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.cleanup_scope_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.cleanup_resume_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.normalized_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.fail_closed_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.contract_violation_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.normalized_sites +
              surface.unwind_cleanup_summary.fail_closed_sites ==
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.deterministic;
  record.async_continuation_ready =
      deterministic_async_continuation_handoff &&
      surface.async_continuation_summary.async_continuation_sites ==
          surface.async_continuation_sites_total &&
      surface.async_continuation_summary.async_keyword_sites ==
          surface.async_continuation_async_keyword_sites_total &&
      surface.async_continuation_summary.async_function_sites ==
          surface.async_continuation_async_function_sites_total &&
      surface.async_continuation_summary.continuation_allocation_sites ==
          surface.async_continuation_allocation_sites_total &&
      surface.async_continuation_summary.continuation_resume_sites ==
          surface.async_continuation_resume_sites_total &&
      surface.async_continuation_summary.continuation_suspend_sites ==
          surface.async_continuation_suspend_sites_total &&
      surface.async_continuation_summary.async_state_machine_sites ==
          surface.async_continuation_state_machine_sites_total &&
      surface.async_continuation_summary.normalized_sites ==
          surface.async_continuation_normalized_sites_total &&
      surface.async_continuation_summary.gate_blocked_sites ==
          surface.async_continuation_gate_blocked_sites_total &&
      surface.async_continuation_summary.contract_violation_sites ==
          surface.async_continuation_contract_violation_sites_total &&
      surface.async_continuation_summary.async_keyword_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.async_function_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.continuation_allocation_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.continuation_resume_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.continuation_suspend_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.async_state_machine_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.normalized_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.gate_blocked_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.contract_violation_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.normalized_sites +
              surface.async_continuation_summary.gate_blocked_sites ==
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.deterministic;
  record.symbol_graph_scope_resolution_ready =
      deterministic_symbol_graph_scope_resolution_handoff &&
      surface.symbol_graph_scope_resolution_summary.global_symbol_nodes ==
          surface.symbol_graph_global_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.function_symbol_nodes ==
          surface.symbol_graph_function_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.interface_symbol_nodes ==
          surface.symbol_graph_interface_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.implementation_symbol_nodes ==
          surface.symbol_graph_implementation_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary
              .interface_property_symbol_nodes ==
          surface.symbol_graph_interface_property_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_property_symbol_nodes ==
          surface.symbol_graph_implementation_property_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.interface_method_symbol_nodes ==
          surface.symbol_graph_interface_method_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_method_symbol_nodes ==
          surface.symbol_graph_implementation_method_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols ==
          surface.symbol_graph_top_level_scope_symbols_total &&
      surface.symbol_graph_scope_resolution_summary.nested_scope_symbols ==
          surface.symbol_graph_nested_scope_symbols_total &&
      surface.symbol_graph_scope_resolution_summary.scope_frames_total ==
          surface.symbol_graph_scope_frames_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_sites ==
          surface
              .symbol_graph_implementation_interface_resolution_sites_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_hits ==
          surface
              .symbol_graph_implementation_interface_resolution_hits_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_misses ==
          surface
              .symbol_graph_implementation_interface_resolution_misses_total &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_sites ==
          surface.symbol_graph_method_resolution_sites_total &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_hits ==
          surface.symbol_graph_method_resolution_hits_total &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_misses ==
          surface.symbol_graph_method_resolution_misses_total &&
      surface.symbol_graph_scope_resolution_summary.symbol_nodes_total() ==
          surface.symbol_graph_scope_resolution_summary
              .top_level_scope_symbols +
              surface.symbol_graph_scope_resolution_summary
                  .nested_scope_symbols &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_hits <=
          surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_sites &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_hits +
              surface.symbol_graph_scope_resolution_summary
                  .implementation_interface_resolution_misses ==
          surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_sites &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_hits <=
          surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_hits +
              surface.symbol_graph_scope_resolution_summary
                  .method_resolution_misses ==
          surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      surface.symbol_graph_scope_resolution_summary.resolution_hits_total() <=
          surface.symbol_graph_scope_resolution_summary
              .resolution_sites_total() &&
      surface.symbol_graph_scope_resolution_summary.resolution_hits_total() +
              surface.symbol_graph_scope_resolution_summary
                  .resolution_misses_total() ==
          surface.symbol_graph_scope_resolution_summary
              .resolution_sites_total() &&
      surface.symbol_graph_scope_resolution_summary.deterministic;
  record.method_lookup_override_conflict_ready =
      deterministic_method_lookup_override_conflict_handoff &&
      surface.method_lookup_override_conflict_summary.method_lookup_sites ==
          surface.method_lookup_override_conflict_lookup_sites_total &&
      surface.method_lookup_override_conflict_summary.method_lookup_hits ==
          surface.method_lookup_override_conflict_lookup_hits_total &&
      surface.method_lookup_override_conflict_summary.method_lookup_misses ==
          surface.method_lookup_override_conflict_lookup_misses_total &&
      surface.method_lookup_override_conflict_summary.override_lookup_sites ==
          surface.method_lookup_override_conflict_override_sites_total &&
      surface.method_lookup_override_conflict_summary.override_lookup_hits ==
          surface.method_lookup_override_conflict_override_hits_total &&
      surface.method_lookup_override_conflict_summary.override_lookup_misses ==
          surface.method_lookup_override_conflict_override_misses_total &&
      surface.method_lookup_override_conflict_summary.override_conflicts ==
          surface.method_lookup_override_conflict_override_conflicts_total &&
      surface.method_lookup_override_conflict_summary
              .unresolved_base_interfaces ==
          surface
              .method_lookup_override_conflict_unresolved_base_interfaces_total &&
      surface.method_lookup_override_conflict_summary.method_lookup_hits <=
          surface.method_lookup_override_conflict_summary.method_lookup_sites &&
      surface.method_lookup_override_conflict_summary.method_lookup_hits +
              surface.method_lookup_override_conflict_summary
                  .method_lookup_misses ==
          surface.method_lookup_override_conflict_summary.method_lookup_sites &&
      surface.method_lookup_override_conflict_summary.override_lookup_hits <=
          surface.method_lookup_override_conflict_summary.override_lookup_sites &&
      surface.method_lookup_override_conflict_summary.override_lookup_hits +
              surface.method_lookup_override_conflict_summary
                  .override_lookup_misses ==
          surface.method_lookup_override_conflict_summary.override_lookup_sites &&
      surface.method_lookup_override_conflict_summary.override_conflicts <=
          surface.method_lookup_override_conflict_summary.override_lookup_hits &&
      surface.method_lookup_override_conflict_summary.deterministic;
  record.property_synthesis_ivar_binding_ready =
      deterministic_property_synthesis_ivar_binding_handoff &&
      surface.property_synthesis_ivar_binding_summary.property_synthesis_sites ==
          surface
              .property_synthesis_ivar_binding_property_synthesis_sites_total &&
      surface.property_synthesis_ivar_binding_summary
              .property_synthesis_explicit_ivar_bindings ==
          surface
              .property_synthesis_ivar_binding_explicit_ivar_bindings_total &&
      surface.property_synthesis_ivar_binding_summary
              .property_synthesis_default_ivar_bindings ==
          surface
              .property_synthesis_ivar_binding_default_ivar_bindings_total &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
          surface.property_synthesis_ivar_binding_ivar_binding_sites_total &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved ==
          surface.property_synthesis_ivar_binding_ivar_binding_resolved_total &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_missing ==
          surface.property_synthesis_ivar_binding_ivar_binding_missing_total &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
          surface.property_synthesis_ivar_binding_ivar_binding_conflicts_total &&
      surface.property_synthesis_ivar_binding_summary
              .property_synthesis_explicit_ivar_bindings +
              surface.property_synthesis_ivar_binding_summary
                  .property_synthesis_default_ivar_bindings ==
          surface.property_synthesis_ivar_binding_summary
              .property_synthesis_sites &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
          surface.property_synthesis_ivar_binding_summary
              .property_synthesis_sites &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved +
              surface.property_synthesis_ivar_binding_summary
                  .ivar_binding_missing +
              surface.property_synthesis_ivar_binding_summary
                  .ivar_binding_conflicts ==
          surface.property_synthesis_ivar_binding_summary
              .ivar_binding_sites &&
      surface.property_synthesis_ivar_binding_summary.deterministic;
  record.id_class_sel_object_pointer_type_checking_ready =
      deterministic_id_class_sel_object_pointer_type_checking_handoff &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_type_sites ==
          surface.id_class_sel_object_pointer_param_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_id_spelling_sites ==
          surface.id_class_sel_object_pointer_param_id_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_class_spelling_sites ==
          surface.id_class_sel_object_pointer_param_class_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_sel_spelling_sites ==
          surface.id_class_sel_object_pointer_param_sel_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_instancetype_spelling_sites ==
          surface
              .id_class_sel_object_pointer_param_instancetype_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_object_pointer_type_sites ==
          surface
              .id_class_sel_object_pointer_param_object_pointer_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_type_sites ==
          surface.id_class_sel_object_pointer_return_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_id_spelling_sites ==
          surface.id_class_sel_object_pointer_return_id_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_class_spelling_sites ==
          surface
              .id_class_sel_object_pointer_return_class_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_sel_spelling_sites ==
          surface.id_class_sel_object_pointer_return_sel_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_instancetype_spelling_sites ==
          surface
              .id_class_sel_object_pointer_return_instancetype_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_object_pointer_type_sites ==
          surface
              .id_class_sel_object_pointer_return_object_pointer_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_type_sites ==
          surface.id_class_sel_object_pointer_property_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_id_spelling_sites ==
          surface.id_class_sel_object_pointer_property_id_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_class_spelling_sites ==
          surface
              .id_class_sel_object_pointer_property_class_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_sel_spelling_sites ==
          surface.id_class_sel_object_pointer_property_sel_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_instancetype_spelling_sites ==
          surface
              .id_class_sel_object_pointer_property_instancetype_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_object_pointer_type_sites ==
          surface
              .id_class_sel_object_pointer_property_object_pointer_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_id_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .param_class_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .param_sel_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .param_instancetype_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .param_object_pointer_type_sites <=
          surface.id_class_sel_object_pointer_type_checking_summary
              .param_type_sites &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_id_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .return_class_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .return_sel_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .return_instancetype_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .return_object_pointer_type_sites <=
          surface.id_class_sel_object_pointer_type_checking_summary
              .return_type_sites &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_id_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .property_class_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .property_sel_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .property_instancetype_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .property_object_pointer_type_sites <=
          surface.id_class_sel_object_pointer_type_checking_summary
              .property_type_sites &&
      surface.id_class_sel_object_pointer_type_checking_summary.deterministic;
  record.passed_validation_count =
      Objc3SemaEvidenceCount(record.unwind_cleanup_ready) +
      Objc3SemaEvidenceCount(record.async_continuation_ready) +
      Objc3SemaEvidenceCount(record.symbol_graph_scope_resolution_ready) +
      Objc3SemaEvidenceCount(record.method_lookup_override_conflict_ready) +
      Objc3SemaEvidenceCount(record.property_synthesis_ivar_binding_ready) +
      Objc3SemaEvidenceCount(
          record.id_class_sel_object_pointer_type_checking_ready);
  record.failed_validation_count =
      record.required_validation_count >= record.passed_validation_count
          ? (record.required_validation_count -
             record.passed_validation_count)
          : record.required_validation_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.control_binding_parity_validation_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_validation_count == 6u &&
      record.passed_validation_count == record.required_validation_count &&
      record.failed_validation_count == 0u;
  return record;
}

inline Objc3SemaAsyncBlockMessageParityValidationReadinessRecord
BuildObjc3SemaAsyncBlockMessageParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_await_lowering_suspension_state_lowering_handoff,
    bool deterministic_block_literal_capture_semantics_handoff,
    bool deterministic_block_abi_invoke_trampoline_handoff,
    bool deterministic_block_storage_escape_handoff,
    bool deterministic_block_copy_dispose_handoff,
    bool deterministic_block_determinism_perf_baseline_handoff,
    bool deterministic_message_send_selector_lowering_handoff) {
  Objc3SemaAsyncBlockMessageParityValidationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  const auto &await_summary =
      surface.await_lowering_suspension_state_lowering_summary;
  record.await_lowering_suspension_state_lowering_ready =
      deterministic_await_lowering_suspension_state_lowering_handoff &&
      await_summary.await_suspension_sites ==
          surface.await_lowering_suspension_state_lowering_sites_total &&
      await_summary.await_keyword_sites ==
          surface
              .await_lowering_suspension_state_lowering_await_keyword_sites_total &&
      await_summary.await_suspension_point_sites ==
          surface
              .await_lowering_suspension_state_lowering_await_suspension_point_sites_total &&
      await_summary.await_resume_sites ==
          surface
              .await_lowering_suspension_state_lowering_await_resume_sites_total &&
      await_summary.await_state_machine_sites ==
          surface
              .await_lowering_suspension_state_lowering_await_state_machine_sites_total &&
      await_summary.await_continuation_sites ==
          surface
              .await_lowering_suspension_state_lowering_await_continuation_sites_total &&
      await_summary.normalized_sites ==
          surface
              .await_lowering_suspension_state_lowering_normalized_sites_total &&
      await_summary.gate_blocked_sites ==
          surface
              .await_lowering_suspension_state_lowering_gate_blocked_sites_total &&
      await_summary.contract_violation_sites ==
          surface
              .await_lowering_suspension_state_lowering_contract_violation_sites_total &&
      await_summary.await_keyword_sites <=
          await_summary.await_suspension_sites &&
      await_summary.await_suspension_point_sites <=
          await_summary.await_suspension_sites &&
      await_summary.await_resume_sites <=
          await_summary.await_suspension_sites &&
      await_summary.await_state_machine_sites <=
          await_summary.await_suspension_point_sites &&
      await_summary.await_continuation_sites <=
          await_summary.await_suspension_point_sites &&
      await_summary.normalized_sites <= await_summary.await_suspension_sites &&
      await_summary.gate_blocked_sites <=
          await_summary.await_suspension_sites &&
      await_summary.contract_violation_sites <=
          await_summary.await_suspension_sites &&
      await_summary.normalized_sites + await_summary.gate_blocked_sites ==
          await_summary.await_suspension_sites &&
      await_summary.deterministic;
  const auto &block_literal_summary =
      surface.block_literal_capture_semantics_summary;
  record.block_literal_capture_semantics_ready =
      deterministic_block_literal_capture_semantics_handoff &&
      block_literal_summary.block_literal_sites ==
          surface.block_literal_capture_semantics_sites_total &&
      block_literal_summary.block_parameter_entries ==
          surface.block_literal_capture_semantics_parameter_entries_total &&
      block_literal_summary.block_capture_entries ==
          surface.block_literal_capture_semantics_capture_entries_total &&
      block_literal_summary.block_body_statement_entries ==
          surface.block_literal_capture_semantics_body_statement_entries_total &&
      block_literal_summary.block_empty_capture_sites ==
          surface.block_literal_capture_semantics_empty_capture_sites_total &&
      block_literal_summary.block_nondeterministic_capture_sites ==
          surface
              .block_literal_capture_semantics_nondeterministic_capture_sites_total &&
      block_literal_summary.block_non_normalized_sites ==
          surface.block_literal_capture_semantics_non_normalized_sites_total &&
      block_literal_summary.contract_violation_sites ==
          surface.block_literal_capture_semantics_contract_violation_sites_total &&
      block_literal_summary.block_empty_capture_sites <=
          block_literal_summary.block_literal_sites &&
      block_literal_summary.block_nondeterministic_capture_sites <=
          block_literal_summary.block_literal_sites &&
      block_literal_summary.block_non_normalized_sites <=
          block_literal_summary.block_literal_sites &&
      block_literal_summary.contract_violation_sites <=
          block_literal_summary.block_literal_sites &&
      block_literal_summary.deterministic;
  const auto &block_abi_summary =
      surface.block_abi_invoke_trampoline_semantics_summary;
  record.block_abi_invoke_trampoline_ready =
      deterministic_block_abi_invoke_trampoline_handoff &&
      block_abi_summary.block_literal_sites ==
          surface.block_abi_invoke_trampoline_sites_total &&
      block_abi_summary.invoke_argument_slots_total ==
          surface.block_abi_invoke_trampoline_invoke_argument_slots_total &&
      block_abi_summary.capture_word_count_total ==
          surface.block_abi_invoke_trampoline_capture_word_count_total &&
      block_abi_summary.parameter_entries_total ==
          surface.block_abi_invoke_trampoline_parameter_entries_total &&
      block_abi_summary.capture_entries_total ==
          surface.block_abi_invoke_trampoline_capture_entries_total &&
      block_abi_summary.body_statement_entries_total ==
          surface.block_abi_invoke_trampoline_body_statement_entries_total &&
      block_abi_summary.descriptor_symbolized_sites ==
          surface.block_abi_invoke_trampoline_descriptor_symbolized_sites_total &&
      block_abi_summary.invoke_trampoline_symbolized_sites ==
          surface.block_abi_invoke_trampoline_invoke_symbolized_sites_total &&
      block_abi_summary.missing_invoke_trampoline_sites ==
          surface.block_abi_invoke_trampoline_missing_invoke_sites_total &&
      block_abi_summary.non_normalized_layout_sites ==
          surface.block_abi_invoke_trampoline_non_normalized_layout_sites_total &&
      block_abi_summary.contract_violation_sites ==
          surface.block_abi_invoke_trampoline_contract_violation_sites_total &&
      block_abi_summary.descriptor_symbolized_sites <=
          block_abi_summary.block_literal_sites &&
      block_abi_summary.invoke_trampoline_symbolized_sites <=
          block_abi_summary.block_literal_sites &&
      block_abi_summary.missing_invoke_trampoline_sites <=
          block_abi_summary.block_literal_sites &&
      block_abi_summary.non_normalized_layout_sites <=
          block_abi_summary.block_literal_sites &&
      block_abi_summary.contract_violation_sites <=
          block_abi_summary.block_literal_sites &&
      block_abi_summary.invoke_trampoline_symbolized_sites +
              block_abi_summary.missing_invoke_trampoline_sites ==
          block_abi_summary.block_literal_sites &&
      block_abi_summary.invoke_argument_slots_total ==
          block_abi_summary.parameter_entries_total &&
      block_abi_summary.capture_word_count_total ==
          block_abi_summary.capture_entries_total &&
      block_abi_summary.deterministic;
  const auto &block_storage_summary =
      surface.block_storage_escape_semantics_summary;
  record.block_storage_escape_ready =
      deterministic_block_storage_escape_handoff &&
      block_storage_summary.block_literal_sites ==
          surface.block_storage_escape_sites_total &&
      block_storage_summary.mutable_capture_count_total ==
          surface.block_storage_escape_mutable_capture_count_total &&
      block_storage_summary.byref_slot_count_total ==
          surface.block_storage_escape_byref_slot_count_total &&
      block_storage_summary.parameter_entries_total ==
          surface.block_storage_escape_parameter_entries_total &&
      block_storage_summary.capture_entries_total ==
          surface.block_storage_escape_capture_entries_total &&
      block_storage_summary.body_statement_entries_total ==
          surface.block_storage_escape_body_statement_entries_total &&
      block_storage_summary.requires_byref_cells_sites ==
          surface.block_storage_escape_requires_byref_cells_sites_total &&
      block_storage_summary.escape_analysis_enabled_sites ==
          surface.block_storage_escape_escape_analysis_enabled_sites_total &&
      block_storage_summary.escape_to_heap_sites ==
          surface.block_storage_escape_escape_to_heap_sites_total &&
      block_storage_summary.escape_profile_normalized_sites ==
          surface.block_storage_escape_escape_profile_normalized_sites_total &&
      block_storage_summary.byref_layout_symbolized_sites ==
          surface.block_storage_escape_byref_layout_symbolized_sites_total &&
      block_storage_summary.contract_violation_sites ==
          surface.block_storage_escape_contract_violation_sites_total &&
      block_storage_summary.requires_byref_cells_sites <=
          block_storage_summary.block_literal_sites &&
      block_storage_summary.escape_analysis_enabled_sites <=
          block_storage_summary.block_literal_sites &&
      block_storage_summary.escape_to_heap_sites <=
          block_storage_summary.block_literal_sites &&
      block_storage_summary.escape_profile_normalized_sites <=
          block_storage_summary.block_literal_sites &&
      block_storage_summary.byref_layout_symbolized_sites <=
          block_storage_summary.block_literal_sites &&
      block_storage_summary.contract_violation_sites <=
          block_storage_summary.block_literal_sites &&
      block_storage_summary.mutable_capture_count_total <=
          block_storage_summary.capture_entries_total &&
      block_storage_summary.byref_slot_count_total <=
          block_storage_summary.mutable_capture_count_total &&
      block_storage_summary.escape_analysis_enabled_sites ==
          block_storage_summary.block_literal_sites &&
      block_storage_summary.deterministic;
  const auto &block_copy_summary = surface.block_copy_dispose_semantics_summary;
  record.block_copy_dispose_ready =
      deterministic_block_copy_dispose_handoff &&
      block_copy_summary.block_literal_sites ==
          surface.block_copy_dispose_sites_total &&
      block_copy_summary.mutable_capture_count_total ==
          surface.block_copy_dispose_mutable_capture_count_total &&
      block_copy_summary.byref_slot_count_total ==
          surface.block_copy_dispose_byref_slot_count_total &&
      block_copy_summary.parameter_entries_total ==
          surface.block_copy_dispose_parameter_entries_total &&
      block_copy_summary.capture_entries_total ==
          surface.block_copy_dispose_capture_entries_total &&
      block_copy_summary.body_statement_entries_total ==
          surface.block_copy_dispose_body_statement_entries_total &&
      block_copy_summary.copy_helper_required_sites ==
          surface.block_copy_dispose_copy_helper_required_sites_total &&
      block_copy_summary.dispose_helper_required_sites ==
          surface.block_copy_dispose_dispose_helper_required_sites_total &&
      block_copy_summary.profile_normalized_sites ==
          surface.block_copy_dispose_profile_normalized_sites_total &&
      block_copy_summary.copy_helper_symbolized_sites ==
          surface.block_copy_dispose_copy_helper_symbolized_sites_total &&
      block_copy_summary.dispose_helper_symbolized_sites ==
          surface.block_copy_dispose_dispose_helper_symbolized_sites_total &&
      block_copy_summary.contract_violation_sites ==
          surface.block_copy_dispose_contract_violation_sites_total &&
      block_copy_summary.copy_helper_required_sites <=
          block_copy_summary.block_literal_sites &&
      block_copy_summary.dispose_helper_required_sites <=
          block_copy_summary.block_literal_sites &&
      block_copy_summary.profile_normalized_sites <=
          block_copy_summary.block_literal_sites &&
      block_copy_summary.copy_helper_symbolized_sites <=
          block_copy_summary.block_literal_sites &&
      block_copy_summary.dispose_helper_symbolized_sites <=
          block_copy_summary.block_literal_sites &&
      block_copy_summary.contract_violation_sites <=
          block_copy_summary.block_literal_sites &&
      block_copy_summary.mutable_capture_count_total <=
          block_copy_summary.capture_entries_total &&
      block_copy_summary.byref_slot_count_total <=
          block_copy_summary.mutable_capture_count_total &&
      block_copy_summary.copy_helper_required_sites <=
          block_copy_summary.dispose_helper_required_sites &&
      block_copy_summary.deterministic;
  const auto &block_determinism_summary =
      surface.block_determinism_perf_baseline_summary;
  record.block_determinism_perf_baseline_ready =
      deterministic_block_determinism_perf_baseline_handoff &&
      block_determinism_summary.block_literal_sites ==
          surface.block_determinism_perf_baseline_sites_total &&
      block_determinism_summary.baseline_weight_total ==
          surface.block_determinism_perf_baseline_weight_total &&
      block_determinism_summary.parameter_entries_total ==
          surface.block_determinism_perf_baseline_parameter_entries_total &&
      block_determinism_summary.capture_entries_total ==
          surface.block_determinism_perf_baseline_capture_entries_total &&
      block_determinism_summary.body_statement_entries_total ==
          surface.block_determinism_perf_baseline_body_statement_entries_total &&
      block_determinism_summary.deterministic_capture_sites ==
          surface
              .block_determinism_perf_baseline_deterministic_capture_sites_total &&
      block_determinism_summary.heavy_tier_sites ==
          surface.block_determinism_perf_baseline_heavy_tier_sites_total &&
      block_determinism_summary.normalized_profile_sites ==
          surface
              .block_determinism_perf_baseline_normalized_profile_sites_total &&
      block_determinism_summary.contract_violation_sites ==
          surface
              .block_determinism_perf_baseline_contract_violation_sites_total &&
      block_determinism_summary.deterministic_capture_sites <=
          block_determinism_summary.block_literal_sites &&
      block_determinism_summary.heavy_tier_sites <=
          block_determinism_summary.block_literal_sites &&
      block_determinism_summary.normalized_profile_sites <=
          block_determinism_summary.block_literal_sites &&
      block_determinism_summary.contract_violation_sites <=
          block_determinism_summary.block_literal_sites &&
      block_determinism_summary.deterministic;
  const auto &message_send_summary =
      surface.message_send_selector_lowering_summary;
  record.message_send_selector_lowering_ready =
      deterministic_message_send_selector_lowering_handoff &&
      message_send_summary.message_send_sites ==
          surface.message_send_selector_lowering_sites_total &&
      message_send_summary.unary_form_sites ==
          surface.message_send_selector_lowering_unary_form_sites_total &&
      message_send_summary.keyword_form_sites ==
          surface.message_send_selector_lowering_keyword_form_sites_total &&
      message_send_summary.selector_lowering_symbol_sites ==
          surface.message_send_selector_lowering_symbol_sites_total &&
      message_send_summary.selector_lowering_piece_entries ==
          surface.message_send_selector_lowering_piece_entries_total &&
      message_send_summary.selector_lowering_argument_piece_entries ==
          surface.message_send_selector_lowering_argument_piece_entries_total &&
      message_send_summary.selector_lowering_normalized_sites ==
          surface.message_send_selector_lowering_normalized_sites_total &&
      message_send_summary.selector_lowering_form_mismatch_sites ==
          surface.message_send_selector_lowering_form_mismatch_sites_total &&
      message_send_summary.selector_lowering_arity_mismatch_sites ==
          surface.message_send_selector_lowering_arity_mismatch_sites_total &&
      message_send_summary.selector_lowering_symbol_mismatch_sites ==
          surface.message_send_selector_lowering_symbol_mismatch_sites_total &&
      message_send_summary.selector_lowering_missing_symbol_sites ==
          surface.message_send_selector_lowering_missing_symbol_sites_total &&
      message_send_summary.selector_lowering_contract_violation_sites ==
          surface.message_send_selector_lowering_contract_violation_sites_total &&
      message_send_summary.unary_form_sites +
              message_send_summary.keyword_form_sites ==
          message_send_summary.message_send_sites &&
      message_send_summary.selector_lowering_symbol_sites <=
          message_send_summary.message_send_sites &&
      message_send_summary.selector_lowering_argument_piece_entries <=
          message_send_summary.selector_lowering_piece_entries &&
      message_send_summary.selector_lowering_normalized_sites <=
          message_send_summary.selector_lowering_symbol_sites &&
      message_send_summary.selector_lowering_form_mismatch_sites <=
          message_send_summary.message_send_sites &&
      message_send_summary.selector_lowering_arity_mismatch_sites <=
          message_send_summary.message_send_sites &&
      message_send_summary.selector_lowering_symbol_mismatch_sites <=
          message_send_summary.message_send_sites &&
      message_send_summary.selector_lowering_missing_symbol_sites <=
          message_send_summary.message_send_sites &&
      message_send_summary.selector_lowering_contract_violation_sites <=
          message_send_summary.message_send_sites &&
      message_send_summary.deterministic;
  record.passed_validation_count =
      Objc3SemaEvidenceCount(
          record.await_lowering_suspension_state_lowering_ready) +
      Objc3SemaEvidenceCount(record.block_literal_capture_semantics_ready) +
      Objc3SemaEvidenceCount(record.block_abi_invoke_trampoline_ready) +
      Objc3SemaEvidenceCount(record.block_storage_escape_ready) +
      Objc3SemaEvidenceCount(record.block_copy_dispose_ready) +
      Objc3SemaEvidenceCount(record.block_determinism_perf_baseline_ready) +
      Objc3SemaEvidenceCount(record.message_send_selector_lowering_ready);
  record.failed_validation_count =
      record.required_validation_count >= record.passed_validation_count
          ? (record.required_validation_count -
             record.passed_validation_count)
          : record.required_validation_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.async_block_message_parity_validation_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_validation_count == 7u &&
      record.passed_validation_count == record.required_validation_count &&
      record.failed_validation_count == 0u;
  return record;
}

inline Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord
BuildObjc3SemaDispatchRuntimeArcParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_dispatch_abi_marshalling_handoff,
    bool deterministic_nil_receiver_semantics_foldability_handoff,
    bool deterministic_super_dispatch_method_family_handoff,
    bool deterministic_runtime_link_host_link_handoff,
    bool deterministic_retain_release_operation_handoff,
    bool deterministic_weak_unowned_semantics_handoff,
    bool deterministic_arc_diagnostics_fixit_handoff,
    bool deterministic_autoreleasepool_scope_handoff) {
  Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  const auto &dispatch_summary = surface.dispatch_abi_marshalling_summary;
  record.dispatch_abi_marshalling_ready =
      deterministic_dispatch_abi_marshalling_handoff &&
      dispatch_summary.message_send_sites ==
          surface.dispatch_abi_marshalling_sites_total &&
      dispatch_summary.receiver_slots ==
          surface.dispatch_abi_marshalling_receiver_slots_total &&
      dispatch_summary.selector_symbol_slots ==
          surface.dispatch_abi_marshalling_selector_symbol_slots_total &&
      dispatch_summary.argument_slots ==
          surface.dispatch_abi_marshalling_argument_slots_total &&
      dispatch_summary.keyword_argument_slots ==
          surface.dispatch_abi_marshalling_keyword_argument_slots_total &&
      dispatch_summary.unary_argument_slots ==
          surface.dispatch_abi_marshalling_unary_argument_slots_total &&
      dispatch_summary.arity_mismatch_sites ==
          surface.dispatch_abi_marshalling_arity_mismatch_sites_total &&
      dispatch_summary.missing_selector_symbol_sites ==
          surface.dispatch_abi_marshalling_missing_selector_symbol_sites_total &&
      dispatch_summary.contract_violation_sites ==
          surface.dispatch_abi_marshalling_contract_violation_sites_total &&
      dispatch_summary.receiver_slots == dispatch_summary.message_send_sites &&
      dispatch_summary.selector_symbol_slots +
              dispatch_summary.missing_selector_symbol_sites ==
          dispatch_summary.message_send_sites &&
      dispatch_summary.keyword_argument_slots +
              dispatch_summary.unary_argument_slots ==
          dispatch_summary.argument_slots &&
      dispatch_summary.keyword_argument_slots <= dispatch_summary.argument_slots &&
      dispatch_summary.unary_argument_slots <= dispatch_summary.argument_slots &&
      dispatch_summary.selector_symbol_slots <=
          dispatch_summary.message_send_sites &&
      dispatch_summary.missing_selector_symbol_sites <=
          dispatch_summary.message_send_sites &&
      dispatch_summary.arity_mismatch_sites <=
          dispatch_summary.message_send_sites &&
      dispatch_summary.contract_violation_sites <=
          dispatch_summary.message_send_sites &&
      dispatch_summary.deterministic;
  const auto &nil_receiver_summary =
      surface.nil_receiver_semantics_foldability_summary;
  record.nil_receiver_semantics_foldability_ready =
      deterministic_nil_receiver_semantics_foldability_handoff &&
      nil_receiver_summary.message_send_sites ==
          surface.nil_receiver_semantics_foldability_sites_total &&
      nil_receiver_summary.receiver_nil_literal_sites ==
          surface
              .nil_receiver_semantics_foldability_receiver_nil_literal_sites_total &&
      nil_receiver_summary.nil_receiver_semantics_enabled_sites ==
          surface.nil_receiver_semantics_foldability_enabled_sites_total &&
      nil_receiver_summary.nil_receiver_foldable_sites ==
          surface.nil_receiver_semantics_foldability_foldable_sites_total &&
      nil_receiver_summary.nil_receiver_runtime_dispatch_required_sites ==
          surface
              .nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total &&
      nil_receiver_summary.non_nil_receiver_sites ==
          surface.nil_receiver_semantics_foldability_non_nil_receiver_sites_total &&
      nil_receiver_summary.contract_violation_sites ==
          surface.nil_receiver_semantics_foldability_contract_violation_sites_total &&
      nil_receiver_summary.receiver_nil_literal_sites ==
          nil_receiver_summary.nil_receiver_semantics_enabled_sites &&
      nil_receiver_summary.nil_receiver_foldable_sites <=
          nil_receiver_summary.nil_receiver_semantics_enabled_sites &&
      nil_receiver_summary.nil_receiver_runtime_dispatch_required_sites +
              nil_receiver_summary.nil_receiver_foldable_sites ==
          nil_receiver_summary.message_send_sites &&
      nil_receiver_summary.nil_receiver_semantics_enabled_sites +
              nil_receiver_summary.non_nil_receiver_sites ==
          nil_receiver_summary.message_send_sites &&
      nil_receiver_summary.contract_violation_sites <=
          nil_receiver_summary.message_send_sites &&
      nil_receiver_summary.deterministic;
  const auto &super_dispatch_summary =
      surface.super_dispatch_method_family_summary;
  record.super_dispatch_method_family_ready =
      deterministic_super_dispatch_method_family_handoff &&
      super_dispatch_summary.message_send_sites ==
          surface.super_dispatch_method_family_sites_total &&
      super_dispatch_summary.receiver_super_identifier_sites ==
          surface
              .super_dispatch_method_family_receiver_super_identifier_sites_total &&
      super_dispatch_summary.super_dispatch_enabled_sites ==
          surface.super_dispatch_method_family_enabled_sites_total &&
      super_dispatch_summary.super_dispatch_requires_class_context_sites ==
          surface
              .super_dispatch_method_family_requires_class_context_sites_total &&
      super_dispatch_summary.method_family_init_sites ==
          surface.super_dispatch_method_family_init_sites_total &&
      super_dispatch_summary.method_family_copy_sites ==
          surface.super_dispatch_method_family_copy_sites_total &&
      super_dispatch_summary.method_family_mutable_copy_sites ==
          surface.super_dispatch_method_family_mutable_copy_sites_total &&
      super_dispatch_summary.method_family_new_sites ==
          surface.super_dispatch_method_family_new_sites_total &&
      super_dispatch_summary.method_family_none_sites ==
          surface.super_dispatch_method_family_none_sites_total &&
      super_dispatch_summary.method_family_returns_retained_result_sites ==
          surface
              .super_dispatch_method_family_returns_retained_result_sites_total &&
      super_dispatch_summary.method_family_returns_related_result_sites ==
          surface
              .super_dispatch_method_family_returns_related_result_sites_total &&
      super_dispatch_summary.contract_violation_sites ==
          surface.super_dispatch_method_family_contract_violation_sites_total &&
      super_dispatch_summary.receiver_super_identifier_sites ==
          super_dispatch_summary.super_dispatch_enabled_sites &&
      super_dispatch_summary.super_dispatch_requires_class_context_sites ==
          super_dispatch_summary.super_dispatch_enabled_sites &&
      super_dispatch_summary.method_family_init_sites +
              super_dispatch_summary.method_family_copy_sites +
              super_dispatch_summary.method_family_mutable_copy_sites +
              super_dispatch_summary.method_family_new_sites +
              super_dispatch_summary.method_family_none_sites ==
          super_dispatch_summary.message_send_sites &&
      super_dispatch_summary.method_family_returns_related_result_sites <=
          super_dispatch_summary.method_family_init_sites &&
      super_dispatch_summary.method_family_returns_retained_result_sites <=
          super_dispatch_summary.message_send_sites &&
      super_dispatch_summary.contract_violation_sites <=
          super_dispatch_summary.message_send_sites &&
      super_dispatch_summary.deterministic;
  const auto &runtime_link_summary = surface.runtime_link_host_link_summary;
  record.runtime_link_host_link_ready =
      deterministic_runtime_link_host_link_handoff &&
      runtime_link_summary.message_send_sites ==
          surface.runtime_link_host_link_message_send_sites_total &&
      runtime_link_summary.runtime_link_required_sites ==
          surface.runtime_link_host_link_required_sites_total &&
      runtime_link_summary.runtime_link_elided_sites ==
          surface.runtime_link_host_link_elided_sites_total &&
      runtime_link_summary.runtime_dispatch_arg_slots ==
          surface.runtime_link_host_link_runtime_dispatch_arg_slots_total &&
      runtime_link_summary.runtime_dispatch_declaration_parameter_count ==
          surface
              .runtime_link_host_link_runtime_dispatch_declaration_parameter_count_total &&
      runtime_link_summary.contract_violation_sites ==
          surface.runtime_link_host_link_contract_violation_sites_total &&
      runtime_link_summary.runtime_dispatch_symbol ==
          surface.runtime_link_host_link_runtime_dispatch_symbol &&
      runtime_link_summary.default_runtime_dispatch_symbol_binding ==
          surface
              .runtime_link_host_link_default_runtime_dispatch_symbol_binding &&
      runtime_link_summary.runtime_link_required_sites +
              runtime_link_summary.runtime_link_elided_sites ==
          runtime_link_summary.message_send_sites &&
      runtime_link_summary.contract_violation_sites <=
          runtime_link_summary.message_send_sites &&
      (runtime_link_summary.message_send_sites == 0 ||
       runtime_link_summary.runtime_dispatch_declaration_parameter_count ==
           runtime_link_summary.runtime_dispatch_arg_slots + 2u) &&
      (runtime_link_summary.default_runtime_dispatch_symbol_binding ==
       (runtime_link_summary.runtime_dispatch_symbol ==
        kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol)) &&
      runtime_link_summary.deterministic;
  const auto &retain_release_summary = surface.retain_release_operation_summary;
  record.retain_release_operation_ready =
      deterministic_retain_release_operation_handoff &&
      retain_release_summary.ownership_qualified_sites ==
          surface.retain_release_operation_ownership_qualified_sites_total &&
      retain_release_summary.retain_insertion_sites ==
          surface.retain_release_operation_retain_insertion_sites_total &&
      retain_release_summary.release_insertion_sites ==
          surface.retain_release_operation_release_insertion_sites_total &&
      retain_release_summary.autorelease_insertion_sites ==
          surface.retain_release_operation_autorelease_insertion_sites_total &&
      retain_release_summary.contract_violation_sites ==
          surface.retain_release_operation_contract_violation_sites_total &&
      retain_release_summary.retain_insertion_sites <=
          retain_release_summary.ownership_qualified_sites +
              retain_release_summary.contract_violation_sites &&
      retain_release_summary.release_insertion_sites <=
          retain_release_summary.ownership_qualified_sites +
              retain_release_summary.contract_violation_sites &&
      retain_release_summary.autorelease_insertion_sites <=
          retain_release_summary.ownership_qualified_sites +
              retain_release_summary.contract_violation_sites &&
      retain_release_summary.deterministic;
  const auto &weak_unowned_summary = surface.weak_unowned_semantics_summary;
  record.weak_unowned_semantics_ready =
      deterministic_weak_unowned_semantics_handoff &&
      weak_unowned_summary.ownership_candidate_sites ==
          surface.weak_unowned_semantics_ownership_candidate_sites_total &&
      weak_unowned_summary.weak_reference_sites ==
          surface.weak_unowned_semantics_weak_reference_sites_total &&
      weak_unowned_summary.unowned_reference_sites ==
          surface.weak_unowned_semantics_unowned_reference_sites_total &&
      weak_unowned_summary.unowned_safe_reference_sites ==
          surface.weak_unowned_semantics_unowned_safe_reference_sites_total &&
      weak_unowned_summary.weak_unowned_conflict_sites ==
          surface.weak_unowned_semantics_conflict_sites_total &&
      weak_unowned_summary.contract_violation_sites ==
          surface.weak_unowned_semantics_contract_violation_sites_total &&
      weak_unowned_summary.unowned_safe_reference_sites <=
          weak_unowned_summary.unowned_reference_sites &&
      weak_unowned_summary.weak_unowned_conflict_sites <=
          weak_unowned_summary.ownership_candidate_sites &&
      weak_unowned_summary.contract_violation_sites <=
          weak_unowned_summary.ownership_candidate_sites +
              weak_unowned_summary.weak_unowned_conflict_sites &&
      weak_unowned_summary.deterministic;
  const auto &arc_diagnostics_summary = surface.arc_diagnostics_fixit_summary;
  record.arc_diagnostics_fixit_ready =
      deterministic_arc_diagnostics_fixit_handoff &&
      arc_diagnostics_summary.ownership_arc_diagnostic_candidate_sites ==
          surface.ownership_arc_diagnostic_candidate_sites_total &&
      arc_diagnostics_summary.ownership_arc_fixit_available_sites ==
          surface.ownership_arc_fixit_available_sites_total &&
      arc_diagnostics_summary.ownership_arc_profiled_sites ==
          surface.ownership_arc_profiled_sites_total &&
      arc_diagnostics_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites ==
          surface.ownership_arc_weak_unowned_conflict_diagnostic_sites_total &&
      arc_diagnostics_summary.ownership_arc_empty_fixit_hint_sites ==
          surface.ownership_arc_empty_fixit_hint_sites_total &&
      arc_diagnostics_summary.contract_violation_sites ==
          surface.ownership_arc_contract_violation_sites_total &&
      arc_diagnostics_summary.ownership_arc_fixit_available_sites <=
          arc_diagnostics_summary.ownership_arc_diagnostic_candidate_sites +
              arc_diagnostics_summary.contract_violation_sites &&
      arc_diagnostics_summary.ownership_arc_profiled_sites <=
          arc_diagnostics_summary.ownership_arc_diagnostic_candidate_sites +
              arc_diagnostics_summary.contract_violation_sites &&
      arc_diagnostics_summary
              .ownership_arc_weak_unowned_conflict_diagnostic_sites <=
          arc_diagnostics_summary.ownership_arc_diagnostic_candidate_sites +
              arc_diagnostics_summary.contract_violation_sites &&
      arc_diagnostics_summary.ownership_arc_empty_fixit_hint_sites <=
          arc_diagnostics_summary.ownership_arc_fixit_available_sites +
              arc_diagnostics_summary.contract_violation_sites &&
      arc_diagnostics_summary.deterministic;
  const auto &autoreleasepool_summary = surface.autoreleasepool_scope_summary;
  record.autoreleasepool_scope_ready =
      deterministic_autoreleasepool_scope_handoff &&
      autoreleasepool_summary.scope_sites ==
          surface.autoreleasepool_scope_sites_total &&
      autoreleasepool_summary.scope_symbolized_sites ==
          surface.autoreleasepool_scope_symbolized_sites_total &&
      autoreleasepool_summary.contract_violation_sites ==
          surface.autoreleasepool_scope_contract_violation_sites_total &&
      autoreleasepool_summary.max_scope_depth ==
          surface.autoreleasepool_scope_max_depth_total &&
      autoreleasepool_summary.scope_symbolized_sites <=
          autoreleasepool_summary.scope_sites &&
      autoreleasepool_summary.contract_violation_sites <=
          autoreleasepool_summary.scope_sites &&
      (autoreleasepool_summary.scope_sites > 0u ||
       autoreleasepool_summary.max_scope_depth == 0u) &&
      autoreleasepool_summary.max_scope_depth <=
          static_cast<unsigned>(autoreleasepool_summary.scope_sites) &&
      autoreleasepool_summary.deterministic;
  record.passed_validation_count =
      Objc3SemaEvidenceCount(record.dispatch_abi_marshalling_ready) +
      Objc3SemaEvidenceCount(
          record.nil_receiver_semantics_foldability_ready) +
      Objc3SemaEvidenceCount(record.super_dispatch_method_family_ready) +
      Objc3SemaEvidenceCount(record.runtime_link_host_link_ready) +
      Objc3SemaEvidenceCount(record.retain_release_operation_ready) +
      Objc3SemaEvidenceCount(record.weak_unowned_semantics_ready) +
      Objc3SemaEvidenceCount(record.arc_diagnostics_fixit_ready) +
      Objc3SemaEvidenceCount(record.autoreleasepool_scope_ready);
  record.failed_validation_count =
      record.required_validation_count >= record.passed_validation_count
          ? (record.required_validation_count -
             record.passed_validation_count)
          : record.required_validation_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.dispatch_runtime_arc_parity_validation_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_validation_count == 8u &&
      record.passed_validation_count == record.required_validation_count &&
      record.failed_validation_count == 0u;
  return record;
}

inline Objc3ParserSemaConformanceEvidenceRecord
BuildObjc3ParserSemaConformanceEvidenceRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3ParserSemaConformanceEvidenceRecord record;
  const Objc3ParserSemaConformanceMatrix &matrix =
      surface.parser_sema_conformance_matrix;
  const Objc3ParserSemaConformanceCorpus &corpus =
      surface.parser_sema_conformance_corpus;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.conformance_matrix_deterministic =
      surface.deterministic_parser_sema_conformance_matrix &&
      matrix.deterministic;
  record.conformance_corpus_deterministic =
      surface.deterministic_parser_sema_conformance_corpus &&
      corpus.deterministic;
  record.declaration_count_evidence_ready =
      matrix.top_level_declaration_count_matches &&
      matrix.global_decl_count_matches && matrix.protocol_decl_count_matches &&
      matrix.interface_decl_count_matches &&
      matrix.implementation_decl_count_matches &&
      matrix.function_decl_count_matches;
  record.member_count_evidence_ready =
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
      matrix.implementation_instance_method_decl_count_matches;
  record.category_function_evidence_ready =
      matrix.interface_category_decl_count_matches &&
      matrix.implementation_category_decl_count_matches &&
      matrix.function_prototype_count_matches &&
      matrix.function_pure_count_matches;
  record.fingerprint_evidence_ready =
      matrix.ast_shape_fingerprint_matches &&
      matrix.ast_top_level_layout_fingerprint_matches &&
      matrix.parser_contract_snapshot_fingerprint_matches;
  record.parser_budget_replay_evidence_ready =
      matrix.parser_diagnostic_budget_consistent &&
      matrix.parser_token_top_level_budget_consistent &&
      matrix.parser_subset_count_consistent &&
      matrix.parser_contract_snapshot_deterministic &&
      matrix.parser_recovery_replay_ready;
  record.passed_matrix_evidence_count =
      Objc3SemaEvidenceCount(matrix.top_level_declaration_count_matches) +
      Objc3SemaEvidenceCount(matrix.global_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.implementation_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.function_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_property_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_class_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.protocol_instance_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_property_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_class_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.interface_instance_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_property_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.implementation_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_class_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_instance_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_category_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_category_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.function_prototype_count_matches) +
      Objc3SemaEvidenceCount(matrix.function_pure_count_matches) +
      Objc3SemaEvidenceCount(matrix.ast_shape_fingerprint_matches) +
      Objc3SemaEvidenceCount(
          matrix.ast_top_level_layout_fingerprint_matches) +
      Objc3SemaEvidenceCount(
          matrix.parser_contract_snapshot_fingerprint_matches) +
      Objc3SemaEvidenceCount(matrix.parser_diagnostic_budget_consistent) +
      Objc3SemaEvidenceCount(matrix.parser_token_top_level_budget_consistent) +
      Objc3SemaEvidenceCount(matrix.parser_subset_count_consistent) +
      Objc3SemaEvidenceCount(matrix.parser_contract_snapshot_deterministic) +
      Objc3SemaEvidenceCount(matrix.parser_recovery_replay_ready);
  record.required_corpus_case_count = corpus.required_case_count;
  record.passed_corpus_case_count = corpus.passed_case_count;
  record.failed_corpus_case_count = corpus.failed_case_count;
  record.corpus_inventory_ready =
      corpus.required_case_count == 5u &&
      corpus.has_top_level_declaration_count_case &&
      corpus.has_snapshot_fingerprint_case &&
      corpus.has_diagnostic_budget_case && corpus.has_subset_count_case &&
      corpus.has_recovery_replay_case;
  record.corpus_cases_passed =
      corpus.passed_case_count == corpus.required_case_count &&
      corpus.failed_case_count == 0u &&
      corpus.top_level_declaration_count_case_passed &&
      corpus.snapshot_fingerprint_case_passed &&
      corpus.diagnostic_budget_case_passed &&
      corpus.subset_count_case_passed && corpus.recovery_replay_case_passed;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_conformance_evidence_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
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
      record.corpus_inventory_ready && record.corpus_cases_passed;
  return record;
}

inline Objc3ParserSemaContractReadinessRecord
BuildObjc3ParserSemaContractReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3ParserSemaContractReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.conformance_evidence_ready =
      surface.deterministic_parser_sema_conformance_evidence_record &&
      IsReadyObjc3ParserSemaConformanceEvidenceRecord(
          surface.parser_sema_conformance_evidence_record);
  record.conformance_matrix_ready =
      record.conformance_evidence_ready &&
      surface.parser_sema_conformance_evidence_record
          .conformance_matrix_deterministic;
  record.conformance_corpus_ready =
      record.conformance_evidence_ready &&
      surface.parser_sema_conformance_evidence_record
          .conformance_corpus_deterministic;
  record.performance_quality_guardrails_ready =
      surface.deterministic_parser_sema_performance_quality_guardrails &&
      surface.parser_sema_performance_quality_guardrails.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          7u,
          surface.parser_sema_performance_quality_guardrails
              .required_guardrail_count,
          surface.parser_sema_performance_quality_guardrails
              .passed_guardrail_count,
          surface.parser_sema_performance_quality_guardrails
              .failed_guardrail_count) &&
      surface.parser_sema_performance_quality_guardrails
          .conformance_matrix_builder_budget_guarded &&
      surface.parser_sema_performance_quality_guardrails
          .conformance_corpus_builder_budget_guarded &&
      surface.parser_sema_performance_quality_guardrails
          .handoff_scaffold_builder_budget_guarded &&
      surface.parser_sema_performance_quality_guardrails
          .matrix_diagnostic_budget_consistent &&
      surface.parser_sema_performance_quality_guardrails
          .matrix_token_top_level_budget_consistent &&
      surface.parser_sema_performance_quality_guardrails
          .matrix_subset_budget_consistent &&
      surface.parser_sema_performance_quality_guardrails
          .corpus_case_budget_consistent;
  record.cross_lane_integration_sync_ready =
      surface.deterministic_parser_sema_cross_lane_integration_sync &&
      surface.parser_sema_cross_lane_integration_sync.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          4u,
          surface.parser_sema_cross_lane_integration_sync.required_sync_count,
          surface.parser_sema_cross_lane_integration_sync.passed_sync_count,
          surface.parser_sema_cross_lane_integration_sync.failed_sync_count) &&
      surface.parser_sema_cross_lane_integration_sync.matrix_consistent &&
      surface.parser_sema_cross_lane_integration_sync.corpus_consistent &&
      surface.parser_sema_cross_lane_integration_sync
          .performance_quality_guardrails_consistent &&
      surface.parser_sema_cross_lane_integration_sync
          .pass_manager_contract_surface_sync;
  record.docs_runbook_sync_ready =
      surface.deterministic_parser_sema_docs_runbook_sync &&
      surface.parser_sema_docs_runbook_sync.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_docs_runbook_sync.required_sync_count,
          surface.parser_sema_docs_runbook_sync.passed_sync_count,
          surface.parser_sema_docs_runbook_sync.failed_sync_count) &&
      surface.parser_sema_docs_runbook_sync.cross_lane_integration_sync_ready &&
      surface.parser_sema_docs_runbook_sync.pass_manager_contract_surface_sync &&
      surface.parser_sema_docs_runbook_sync.parity_surface_sync;
  record.release_candidate_replay_dry_run_ready =
      surface.deterministic_parser_sema_release_candidate_replay_dry_run &&
      surface.parser_sema_release_candidate_replay_dry_run.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_release_candidate_replay_dry_run
              .required_sync_count,
          surface.parser_sema_release_candidate_replay_dry_run
              .passed_sync_count,
          surface.parser_sema_release_candidate_replay_dry_run
              .failed_sync_count) &&
      surface.parser_sema_release_candidate_replay_dry_run
          .docs_runbook_sync_ready &&
      surface.parser_sema_release_candidate_replay_dry_run
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_release_candidate_replay_dry_run.replay_surface_sync;
  record.advanced_core_shard1_ready =
      surface.deterministic_parser_sema_advanced_core_shard1 &&
      surface.parser_sema_advanced_core_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_core_shard1.required_sync_count,
          surface.parser_sema_advanced_core_shard1.passed_sync_count,
          surface.parser_sema_advanced_core_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_core_shard1
          .release_candidate_replay_dry_run_ready &&
      surface.parser_sema_advanced_core_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_core_shard1.shard_surface_sync;
  record.advanced_contract_rejection_shard1_ready =
      surface.deterministic_parser_sema_advanced_contract_rejection_shard1 &&
      surface.parser_sema_advanced_contract_rejection_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_contract_rejection_shard1
              .required_sync_count,
          surface.parser_sema_advanced_contract_rejection_shard1
              .passed_sync_count,
          surface.parser_sema_advanced_contract_rejection_shard1
              .failed_sync_count) &&
      surface.parser_sema_advanced_contract_rejection_shard1
          .advanced_core_shard1_ready &&
      surface.parser_sema_advanced_contract_rejection_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_contract_rejection_shard1
          .shard_surface_sync;
  record.advanced_diagnostics_shard1_ready =
      surface.deterministic_parser_sema_advanced_diagnostics_shard1 &&
      surface.parser_sema_advanced_diagnostics_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_diagnostics_shard1.required_sync_count,
          surface.parser_sema_advanced_diagnostics_shard1.passed_sync_count,
          surface.parser_sema_advanced_diagnostics_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_diagnostics_shard1
          .advanced_contract_rejection_shard1_ready &&
      surface.parser_sema_advanced_diagnostics_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_diagnostics_shard1.shard_surface_sync;
  record.advanced_conformance_shard1_ready =
      surface.deterministic_parser_sema_advanced_conformance_shard1 &&
      surface.parser_sema_advanced_conformance_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_conformance_shard1.required_sync_count,
          surface.parser_sema_advanced_conformance_shard1.passed_sync_count,
          surface.parser_sema_advanced_conformance_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_conformance_shard1
          .advanced_diagnostics_shard1_ready &&
      surface.parser_sema_advanced_conformance_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_conformance_shard1.shard_surface_sync;
  record.advanced_integration_shard1_ready =
      surface.deterministic_parser_sema_advanced_integration_shard1 &&
      surface.parser_sema_advanced_integration_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_integration_shard1.required_sync_count,
          surface.parser_sema_advanced_integration_shard1.passed_sync_count,
          surface.parser_sema_advanced_integration_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_integration_shard1
          .advanced_conformance_shard1_ready &&
      surface.parser_sema_advanced_integration_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_integration_shard1.shard_surface_sync;
  record.advanced_performance_shard1_ready =
      surface.deterministic_parser_sema_advanced_performance_shard1 &&
      surface.parser_sema_advanced_performance_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_performance_shard1.required_sync_count,
          surface.parser_sema_advanced_performance_shard1.passed_sync_count,
          surface.parser_sema_advanced_performance_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_performance_shard1
          .advanced_integration_shard1_ready &&
      surface.parser_sema_advanced_performance_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_performance_shard1.shard_surface_sync;
  record.advanced_core_shard2_ready =
      surface.deterministic_parser_sema_advanced_core_shard2 &&
      surface.parser_sema_advanced_core_shard2.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_core_shard2.required_sync_count,
          surface.parser_sema_advanced_core_shard2.passed_sync_count,
          surface.parser_sema_advanced_core_shard2.failed_sync_count) &&
      surface.parser_sema_advanced_core_shard2
          .advanced_performance_shard1_ready &&
      surface.parser_sema_advanced_core_shard2
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_core_shard2.shard_surface_sync;
  record.advanced_contract_rejection_shard2_ready =
      surface.deterministic_parser_sema_advanced_contract_rejection_shard2 &&
      surface.parser_sema_advanced_contract_rejection_shard2.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_contract_rejection_shard2
              .required_sync_count,
          surface.parser_sema_advanced_contract_rejection_shard2
              .passed_sync_count,
          surface.parser_sema_advanced_contract_rejection_shard2
              .failed_sync_count) &&
      surface.parser_sema_advanced_contract_rejection_shard2
          .advanced_core_shard2_ready &&
      surface.parser_sema_advanced_contract_rejection_shard2
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_contract_rejection_shard2
          .shard_surface_sync;
  record.advanced_diagnostics_shard2_ready =
      surface.deterministic_parser_sema_advanced_diagnostics_shard2 &&
      surface.parser_sema_advanced_diagnostics_shard2.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_diagnostics_shard2.required_sync_count,
          surface.parser_sema_advanced_diagnostics_shard2.passed_sync_count,
          surface.parser_sema_advanced_diagnostics_shard2.failed_sync_count) &&
      surface.parser_sema_advanced_diagnostics_shard2
          .advanced_contract_rejection_shard2_ready &&
      surface.parser_sema_advanced_diagnostics_shard2
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_diagnostics_shard2.shard_surface_sync;
  record.integration_closeout_ready =
      surface.deterministic_parser_sema_integration_closeout_signoff &&
      surface.parser_sema_integration_closeout_signoff.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_integration_closeout_signoff.required_sync_count,
          surface.parser_sema_integration_closeout_signoff.passed_sync_count,
          surface.parser_sema_integration_closeout_signoff.failed_sync_count) &&
      surface.parser_sema_integration_closeout_signoff
          .advanced_diagnostics_shard2_ready &&
      surface.parser_sema_integration_closeout_signoff
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_integration_closeout_signoff.gate_signoff_surface_sync;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_contract_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.parity_validation_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.conformance_evidence_ready &&
      record.conformance_matrix_ready && record.conformance_corpus_ready &&
      record.performance_quality_guardrails_ready &&
      record.cross_lane_integration_sync_ready &&
      record.docs_runbook_sync_ready &&
      record.release_candidate_replay_dry_run_ready &&
      record.advanced_core_shard1_ready &&
      record.advanced_contract_rejection_shard1_ready &&
      record.advanced_diagnostics_shard1_ready &&
      record.advanced_conformance_shard1_ready &&
      record.advanced_integration_shard1_ready &&
      record.advanced_performance_shard1_ready &&
      record.advanced_core_shard2_ready &&
      record.advanced_contract_rejection_shard2_ready &&
      record.advanced_diagnostics_shard2_ready &&
      record.integration_closeout_ready;
  return record;
}

#include "sema/objc3_sema_pass_manager_closeout_readiness.inc"

Objc3SemaParityCloseoutPublicationReadinessRecord
BuildObjc3SemaParityCloseoutPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    bool pass_manager_executed,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaCloseoutSurfaceReadinessRecord
BuildObjc3SemaCloseoutSurfaceReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

inline Objc3SemaCloseoutSignoffRecord BuildObjc3SemaCloseoutSignoffRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  const Objc3SemaParityCloseoutPublicationReadinessRecord
      &closeout_readiness =
          surface.parity_closeout_publication_readiness_record;
  return BuildObjc3SemaCloseoutSignoffRecord(
      input,
      surface.ready && surface.deterministic_parity_validation_record &&
          IsReadyObjc3SemaParityValidationRecord(
              surface.parity_validation_record),
      closeout_readiness.parser_sema_contract_ready,
      surface.deterministic_pass_manager_publication_record &&
          IsReadyObjc3SemaPassManagerPublicationRecord(
              surface.pass_manager_publication_record),
      surface.deterministic_type_metadata_publication_record &&
          IsReadyObjc3SemaTypeMetadataPublicationRecord(
              surface.type_metadata_publication_record),
      closeout_readiness.diagnostics_publication_ready,
      closeout_readiness.pass_flow_recovery_ready,
      closeout_readiness.mapping_summaries_ready,
      closeout_readiness.typed_semantic_handoffs_ready);
}

#include "sema/objc3_sema_pass_manager_surface_readiness.inc"

inline bool IsReadyObjc3SemaParityContractSurface(const Objc3SemaParityContractSurface &surface) {
  return IsReadyObjc3SemaParityContractSurfaceReadinessGates(
      BuildObjc3SemaParityContractSurfaceReadinessGates(surface));
}
