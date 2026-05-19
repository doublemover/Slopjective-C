#pragma once

#include <cstddef>
#include <string>

#include "sema/objc3_sema_canonical_literal_contract.h"
#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_pass_flow_core_contract.h"
#include "sema/objc3_sema_pass_flow_diagnostics_contract.h"

struct Objc3SemaPassManagerInput {
  const Objc3ParsedProgram *program = nullptr;
  const Objc3ParserContractSnapshot *parser_contract_snapshot = nullptr;
  Objc3SemanticValidationOptions validation_options;
  Objc3SemaLanguageProfile language_profile = Objc3SemaLanguageProfile::Canonical;
  Objc3SemaCanonicalLiteralRejectionCounts canonical_literal_rejection_counts;
  Objc3SemaDiagnosticsBus diagnostics_bus;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner = kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  bool recovery_counts_as_success = false;
};

struct Objc3ParserSemaHandoffOwnerRecord {
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner = kObjc3SemaTypedSemanticHandoffOwner;
  std::string parser_sema_contract_handoff_owner =
      kObjc3ParserSemaContractHandoffOwner;
  std::string snapshot_normalization_owner =
      kObjc3ParserSemaSnapshotNormalizationOwner;
  std::string canonical_rejection_owner =
      kObjc3ParserSemaCanonicalRejectionOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  bool strict_snapshot_normalization_rejection = true;
  bool strict_canonical_literal_rejection = true;
  bool parser_contract_snapshot_supplied = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3ParserSemaHandoffOwnerRecord(
    const Objc3ParserSemaHandoffOwnerRecord &record) {
  return Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(record.snapshot_normalization_owner) &&
         Objc3SemaOwnerIsExplicit(record.canonical_rejection_owner) &&
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
         record.strict_snapshot_normalization_rejection &&
         record.strict_canonical_literal_rejection && record.deterministic;
}

inline Objc3ParserSemaHandoffOwnerRecord
BuildObjc3ParserSemaHandoffOwnerRecord(const Objc3SemaPassManagerInput &input) {
  Objc3ParserSemaHandoffOwnerRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.parser_contract_snapshot_supplied =
      input.parser_contract_snapshot != nullptr;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.snapshot_normalization_owner) &&
      Objc3SemaOwnerIsExplicit(record.canonical_rejection_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.strict_snapshot_normalization_rejection &&
      record.strict_canonical_literal_rejection;
  return record;
}

struct Objc3SemaPassManagerPublicationRecord {
  Objc3ParserSemaHandoffOwnerRecord parser_sema_handoff_owner_record;
  Objc3SemaDiagnosticsPublicationRecord diagnostics_publication_record;
  std::string pass_manager_publication_owner =
      kObjc3SemaPassManagerPublicationOwner;
  std::string pass_flow_owner = kObjc3SemaStageInputOwner;
  std::string type_metadata_publication_owner =
      kObjc3SemaTypeMetadataPublicationOwner;
  std::string diagnostic_handoff_owner = kObjc3SemaDiagnosticHandoffOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  bool parser_sema_handoff_owner_ready = false;
  bool pass_flow_summary_ready = false;
  bool semantic_diagnostics_ready = false;
  bool type_metadata_handoff_ready = false;
  bool diagnostics_publication_ready = false;
  bool diagnostics_publication_record_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaPassManagerPublicationRecord(
    const Objc3SemaPassManagerPublicationRecord &record) {
  return IsReadyObjc3ParserSemaHandoffOwnerRecord(
             record.parser_sema_handoff_owner_record) &&
         Objc3SemaOwnerIsExplicit(record.pass_manager_publication_owner) &&
         Objc3SemaOwnerIsExplicit(record.pass_flow_owner) &&
         Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
         Objc3SemaOwnerIsExplicit(record.diagnostic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
         record.parser_sema_handoff_owner_ready &&
         record.pass_flow_summary_ready && record.semantic_diagnostics_ready &&
         record.type_metadata_handoff_ready &&
         record.diagnostics_publication_ready &&
         record.diagnostics_publication_record_ready && record.deterministic;
}

inline Objc3SemaPassManagerPublicationRecord
BuildObjc3SemaPassManagerPublicationRecord(
    const Objc3ParserSemaHandoffOwnerRecord &parser_owner_record,
    const Objc3SemaPassFlowSummary &pass_flow_summary,
    const Objc3SemaDiagnosticsPublicationRecord &diagnostics_record,
    bool deterministic_type_metadata_handoff) {
  Objc3SemaPassManagerPublicationRecord record;
  record.parser_sema_handoff_owner_record = parser_owner_record;
  record.diagnostics_publication_record = diagnostics_record;
  record.pass_flow_owner = pass_flow_summary.stage_input_owner;
  record.diagnostic_handoff_owner = pass_flow_summary.diagnostic_handoff_owner;
  record.owner_model = pass_flow_summary.owner_model;
  record.strict_no_retired_route = pass_flow_summary.strict_no_retired_route;
  record.strict_no_compatibility = pass_flow_summary.strict_no_compatibility;
  record.parser_sema_handoff_owner_ready =
      IsReadyObjc3ParserSemaHandoffOwnerRecord(parser_owner_record);
  record.pass_flow_summary_ready =
      IsReadyObjc3SemaPassFlowSummary(pass_flow_summary);
  record.semantic_diagnostics_ready =
      diagnostics_record.semantic_diagnostics_ready;
  record.type_metadata_handoff_ready = deterministic_type_metadata_handoff;
  record.diagnostics_publication_ready =
      pass_flow_summary.diagnostics_hardening_satisfied &&
      pass_flow_summary.diagnostics_bus_publish_consistent &&
      pass_flow_summary.diagnostics_canonicalized;
  record.diagnostics_publication_record_ready =
      IsReadyObjc3SemaDiagnosticsPublicationRecord(diagnostics_record);
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.pass_manager_publication_owner) &&
      Objc3SemaOwnerIsExplicit(record.pass_flow_owner) &&
      Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
      Objc3SemaOwnerIsExplicit(record.diagnostic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.parser_sema_handoff_owner_ready &&
      record.pass_flow_summary_ready && record.semantic_diagnostics_ready &&
      record.type_metadata_handoff_ready &&
      record.diagnostics_publication_ready &&
      record.diagnostics_publication_record_ready;
  return record;
}

struct Objc3SemaTypeMetadataPublicationRecord {
  std::string integration_surface_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string type_metadata_publication_owner =
      kObjc3SemaTypeMetadataPublicationOwner;
  std::string atomic_vector_mapping_publication_owner =
      kObjc3SemaAtomicVectorMappingPublicationOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  std::size_t integration_surface_global_count = 0;
  std::size_t integration_surface_function_count = 0;
  std::size_t integration_surface_interface_count = 0;
  std::size_t integration_surface_implementation_count = 0;
  std::size_t type_metadata_global_entries = 0;
  std::size_t type_metadata_function_entries = 0;
  std::size_t type_metadata_interface_entries = 0;
  std::size_t type_metadata_implementation_entries = 0;
  bool cardinality_consistent = false;
  bool interface_implementation_handoff_ready = false;
  bool protocol_category_composition_handoff_ready = false;
  bool class_protocol_category_linking_handoff_ready = false;
  bool selector_normalization_handoff_ready = false;
  bool deterministic_type_metadata_handoff = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaTypeMetadataPublicationRecord(
    const Objc3SemaTypeMetadataPublicationRecord &record) {
  return Objc3SemaOwnerIsExplicit(record.integration_surface_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.atomic_vector_mapping_publication_owner) &&
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
         record.cardinality_consistent &&
         record.interface_implementation_handoff_ready &&
         record.protocol_category_composition_handoff_ready &&
         record.class_protocol_category_linking_handoff_ready &&
         record.selector_normalization_handoff_ready &&
         record.deterministic_type_metadata_handoff && record.deterministic;
}

inline Objc3SemaTypeMetadataPublicationRecord
BuildObjc3SemaTypeMetadataPublicationRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff,
    bool deterministic_type_metadata_handoff,
    bool deterministic_interface_implementation_handoff,
    bool deterministic_protocol_category_composition_handoff,
    bool deterministic_class_protocol_category_linking_handoff,
    bool deterministic_selector_normalization_handoff) {
  Objc3SemaTypeMetadataPublicationRecord record;
  record.integration_surface_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.integration_surface_global_count = integration_surface.globals.size();
  record.integration_surface_function_count =
      integration_surface.functions.size();
  record.integration_surface_interface_count =
      integration_surface.interfaces.size();
  record.integration_surface_implementation_count =
      integration_surface.implementations.size();
  record.type_metadata_global_entries =
      type_metadata_handoff.global_names_lexicographic.size();
  record.type_metadata_function_entries =
      type_metadata_handoff.functions_lexicographic.size();
  record.type_metadata_interface_entries =
      type_metadata_handoff.interfaces_lexicographic.size();
  record.type_metadata_implementation_entries =
      type_metadata_handoff.implementations_lexicographic.size();
  record.cardinality_consistent =
      record.integration_surface_global_count ==
          record.type_metadata_global_entries &&
      record.integration_surface_function_count ==
          record.type_metadata_function_entries &&
      record.integration_surface_interface_count ==
          record.type_metadata_interface_entries &&
      record.integration_surface_implementation_count ==
          record.type_metadata_implementation_entries;
  record.interface_implementation_handoff_ready =
      deterministic_interface_implementation_handoff &&
      type_metadata_handoff.interface_implementation_summary.deterministic;
  record.protocol_category_composition_handoff_ready =
      deterministic_protocol_category_composition_handoff &&
      type_metadata_handoff.protocol_category_composition_summary.deterministic;
  record.class_protocol_category_linking_handoff_ready =
      deterministic_class_protocol_category_linking_handoff &&
      type_metadata_handoff.class_protocol_category_linking_summary
          .deterministic;
  record.selector_normalization_handoff_ready =
      deterministic_selector_normalization_handoff &&
      type_metadata_handoff.selector_normalization_summary.deterministic;
  record.deterministic_type_metadata_handoff =
      deterministic_type_metadata_handoff;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.integration_surface_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.cardinality_consistent &&
      record.interface_implementation_handoff_ready &&
      record.protocol_category_composition_handoff_ready &&
      record.class_protocol_category_linking_handoff_ready &&
      record.selector_normalization_handoff_ready &&
      record.deterministic_type_metadata_handoff;
  return record;
}
