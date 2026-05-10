#pragma once

#include <cstddef>
#include <string>

#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_contract_core.h"
#include "sema/objc3_sema_pass_flow_core_contract.h"

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
