#pragma once

#include <string>

#include "sema/objc3_sema_pass_manager_publication_contract.h"

struct Objc3SemaParityCloseoutPublicationReadinessRecord {
  std::string parity_closeout_publication_readiness_owner =
      kObjc3SemaParityCloseoutPublicationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string pass_manager_publication_owner =
      kObjc3SemaPassManagerPublicationOwner;
  std::string type_metadata_publication_owner =
      kObjc3SemaTypeMetadataPublicationOwner;
  std::string type_metadata_mapping_readiness_owner =
      kObjc3SemaTypeMetadataMappingReadinessOwner;
  std::string typed_semantic_handoff_publication_owner =
      kObjc3SemaTypedSemanticHandoffPublicationOwner;
  std::string parser_sema_contract_readiness_owner =
      kObjc3ParserSemaContractReadinessOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  bool pass_manager_executed = false;
  bool parser_sema_contract_ready = false;
  bool pass_flow_summary_ready = false;
  bool publication_records_ready = false;
  bool diagnostics_publication_ready = false;
  bool pass_flow_recovery_ready = false;
  bool type_metadata_cardinality_ready = false;
  bool typed_semantic_handoffs_ready = false;
  bool mapping_summaries_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaParityCloseoutPublicationReadinessRecord(
    const Objc3SemaParityCloseoutPublicationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.parity_closeout_publication_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.pass_manager_publication_owner) &&
         Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.type_metadata_mapping_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.typed_semantic_handoff_publication_owner) &&
         Objc3SemaOwnerIsExplicit(
             record.parser_sema_contract_readiness_owner) &&
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
         record.pass_manager_executed && record.parser_sema_contract_ready &&
         record.pass_flow_summary_ready && record.publication_records_ready &&
         record.diagnostics_publication_ready &&
         record.pass_flow_recovery_ready &&
         record.type_metadata_cardinality_ready &&
         record.typed_semantic_handoffs_ready &&
         record.mapping_summaries_ready && record.deterministic;
}

struct Objc3SemaParityValidationRecord {
  std::string parity_validation_owner = kObjc3SemaParityValidationOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string pass_manager_publication_owner =
      kObjc3SemaPassManagerPublicationOwner;
  std::string type_metadata_publication_owner =
      kObjc3SemaTypeMetadataPublicationOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  bool pass_manager_executed = false;
  bool parser_sema_contract_ready = false;
  bool pass_flow_summary_ready = false;
  bool publication_records_ready = false;
  bool diagnostics_publication_ready = false;
  bool pass_flow_recovery_ready = false;
  bool type_metadata_cardinality_ready = false;
  bool typed_semantic_handoffs_ready = false;
  bool mapping_summaries_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaParityValidationRecord(
    const Objc3SemaParityValidationRecord &record) {
  return Objc3SemaOwnerIsExplicit(record.parity_validation_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.pass_manager_publication_owner) &&
         Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
         record.pass_manager_executed && record.parser_sema_contract_ready &&
         record.pass_flow_summary_ready && record.publication_records_ready &&
         record.diagnostics_publication_ready &&
         record.pass_flow_recovery_ready &&
         record.type_metadata_cardinality_ready &&
         record.typed_semantic_handoffs_ready &&
         record.mapping_summaries_ready && record.deterministic;
}

inline Objc3SemaParityValidationRecord BuildObjc3SemaParityValidationRecord(
    const Objc3SemaPassManagerInput &input,
    bool pass_manager_executed,
    bool parser_sema_contract_ready,
    bool pass_flow_summary_ready,
    bool publication_records_ready,
    bool diagnostics_publication_ready,
    bool pass_flow_recovery_ready,
    bool type_metadata_cardinality_ready,
    bool typed_semantic_handoffs_ready,
    bool mapping_summaries_ready) {
  Objc3SemaParityValidationRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.pass_manager_executed = pass_manager_executed;
  record.parser_sema_contract_ready = parser_sema_contract_ready;
  record.pass_flow_summary_ready = pass_flow_summary_ready;
  record.publication_records_ready = publication_records_ready;
  record.diagnostics_publication_ready = diagnostics_publication_ready;
  record.pass_flow_recovery_ready = pass_flow_recovery_ready;
  record.type_metadata_cardinality_ready = type_metadata_cardinality_ready;
  record.typed_semantic_handoffs_ready = typed_semantic_handoffs_ready;
  record.mapping_summaries_ready = mapping_summaries_ready;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.parity_validation_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.pass_manager_publication_owner) &&
      Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.pass_manager_executed && record.parser_sema_contract_ready &&
      record.pass_flow_summary_ready && record.publication_records_ready &&
      record.diagnostics_publication_ready &&
      record.pass_flow_recovery_ready &&
      record.type_metadata_cardinality_ready &&
      record.typed_semantic_handoffs_ready && record.mapping_summaries_ready;
  return record;
}

inline Objc3SemaParityValidationRecord BuildObjc3SemaParityValidationRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityCloseoutPublicationReadinessRecord &readiness) {
  return BuildObjc3SemaParityValidationRecord(
      input,
      readiness.pass_manager_executed,
      readiness.parser_sema_contract_ready,
      readiness.pass_flow_summary_ready,
      readiness.publication_records_ready,
      readiness.diagnostics_publication_ready,
      readiness.pass_flow_recovery_ready,
      readiness.type_metadata_cardinality_ready,
      readiness.typed_semantic_handoffs_ready,
      readiness.mapping_summaries_ready);
}

struct Objc3SemaCloseoutSignoffRecord {
  std::string closeout_signoff_owner = kObjc3SemaCloseoutSignoffOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string parity_validation_owner = kObjc3SemaParityValidationOwner;
  std::string pass_manager_publication_owner =
      kObjc3SemaPassManagerPublicationOwner;
  std::string type_metadata_publication_owner =
      kObjc3SemaTypeMetadataPublicationOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  bool parity_validation_ready = false;
  bool parser_sema_closeout_ready = false;
  bool pass_manager_publication_ready = false;
  bool type_metadata_publication_ready = false;
  bool diagnostics_publication_ready = false;
  bool pass_flow_recovery_ready = false;
  bool type_metadata_handoff_ready = false;
  bool typed_semantic_handoffs_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaCloseoutSignoffRecord(
    const Objc3SemaCloseoutSignoffRecord &record) {
  return Objc3SemaOwnerIsExplicit(record.closeout_signoff_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.parity_validation_owner) &&
         Objc3SemaOwnerIsExplicit(record.pass_manager_publication_owner) &&
         Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
         record.parity_validation_ready && record.parser_sema_closeout_ready &&
         record.pass_manager_publication_ready &&
         record.type_metadata_publication_ready &&
         record.diagnostics_publication_ready &&
         record.pass_flow_recovery_ready && record.type_metadata_handoff_ready &&
         record.typed_semantic_handoffs_ready && record.deterministic;
}

inline Objc3SemaCloseoutSignoffRecord BuildObjc3SemaCloseoutSignoffRecord(
    const Objc3SemaPassManagerInput &input,
    bool parity_validation_ready,
    bool parser_sema_closeout_ready,
    bool pass_manager_publication_ready,
    bool type_metadata_publication_ready,
    bool diagnostics_publication_ready,
    bool pass_flow_recovery_ready,
    bool type_metadata_handoff_ready,
    bool typed_semantic_handoffs_ready) {
  Objc3SemaCloseoutSignoffRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.parity_validation_ready = parity_validation_ready;
  record.parser_sema_closeout_ready = parser_sema_closeout_ready;
  record.pass_manager_publication_ready = pass_manager_publication_ready;
  record.type_metadata_publication_ready = type_metadata_publication_ready;
  record.diagnostics_publication_ready = diagnostics_publication_ready;
  record.pass_flow_recovery_ready = pass_flow_recovery_ready;
  record.type_metadata_handoff_ready = type_metadata_handoff_ready;
  record.typed_semantic_handoffs_ready = typed_semantic_handoffs_ready;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.closeout_signoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.parity_validation_owner) &&
      Objc3SemaOwnerIsExplicit(record.pass_manager_publication_owner) &&
      Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.parity_validation_ready && record.parser_sema_closeout_ready &&
      record.pass_manager_publication_ready &&
      record.type_metadata_publication_ready &&
      record.diagnostics_publication_ready &&
      record.pass_flow_recovery_ready && record.type_metadata_handoff_ready &&
      record.typed_semantic_handoffs_ready;
  return record;
}
