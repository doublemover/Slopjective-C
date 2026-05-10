#pragma once

#include <cstddef>
#include <string>

#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_pass_flow_core_contract.h"

struct Objc3SemaCoreSemanticParityPublicationReadinessRecord {
  std::string core_semantic_parity_publication_readiness_owner =
      kObjc3SemaCoreSemanticParityPublicationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
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
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
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
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
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
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
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
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
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
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
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
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
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
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
         record.required_publication_count == 3u &&
         record.passed_publication_count == record.required_publication_count &&
         record.failed_publication_count == 0u &&
         record.actor_isolation_sendability_ready &&
         record.task_runtime_cancellation_ready &&
         record.concurrency_replay_race_guard_ready && record.deterministic;
}
