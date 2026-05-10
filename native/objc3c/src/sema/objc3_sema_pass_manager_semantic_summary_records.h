#pragma once

#include <string>

#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_pass_flow_core_contract.h"

struct Objc3SemaCoreSemanticSummaryReadinessRecord {
  std::string core_semantic_summary_readiness_owner =
      kObjc3SemaCoreSemanticSummaryReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
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
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
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
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
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
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
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
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
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
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
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
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
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
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
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
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
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
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
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
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
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
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
         record.cross_module_conformance_ready &&
         record.throws_propagation_ready && record.deterministic;
}
