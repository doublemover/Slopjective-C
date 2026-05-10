#pragma once

#include <string>

#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_pass_flow_core_contract.h"

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
