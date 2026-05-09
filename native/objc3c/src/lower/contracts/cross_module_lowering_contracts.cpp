#include "lower/contracts/cross_module_lowering_contracts.h"

#include "lower/metadata/lowering_metadata_helpers.h"

#include <string>

bool IsValidObjc3ModuleImportGraphLoweringContract(
    const Objc3ModuleImportGraphLoweringContract &contract) {
  if (contract.import_edge_candidate_sites >
          contract.module_import_graph_sites ||
      contract.namespace_segment_sites > contract.module_import_graph_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites > contract.module_import_graph_sites ||
      contract.normalized_sites > contract.module_import_graph_sites ||
      contract.contract_violation_sites > contract.module_import_graph_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.module_import_graph_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ModuleImportGraphLoweringReplayKey(
    const Objc3ModuleImportGraphLoweringContract &contract) {
  return std::string("module_import_graph_sites=") +
         std::to_string(contract.module_import_graph_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ModuleImportGraphLoweringLaneContract;
}

bool IsValidObjc3NamespaceCollisionShadowingLoweringContract(
    const Objc3NamespaceCollisionShadowingLoweringContract &contract) {
  if (contract.namespace_segment_sites >
          contract.namespace_collision_shadowing_sites ||
      contract.import_edge_candidate_sites >
          contract.namespace_collision_shadowing_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites >
          contract.namespace_collision_shadowing_sites ||
      contract.normalized_sites >
          contract.namespace_collision_shadowing_sites ||
      contract.contract_violation_sites >
          contract.namespace_collision_shadowing_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites !=
           contract.namespace_collision_shadowing_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3NamespaceCollisionShadowingLoweringReplayKey(
    const Objc3NamespaceCollisionShadowingLoweringContract &contract) {
  return std::string("namespace_collision_shadowing_sites=") +
         std::to_string(contract.namespace_collision_shadowing_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3NamespaceCollisionShadowingLoweringLaneContract;
}

bool IsValidObjc3PublicPrivateApiPartitionLoweringContract(
    const Objc3PublicPrivateApiPartitionLoweringContract &contract) {
  if (contract.namespace_segment_sites >
          contract.public_private_api_partition_sites ||
      contract.import_edge_candidate_sites >
          contract.public_private_api_partition_sites ||
      contract.object_pointer_type_sites <
          contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites >
          contract.public_private_api_partition_sites ||
      contract.normalized_sites > contract.public_private_api_partition_sites ||
      contract.contract_violation_sites >
          contract.public_private_api_partition_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites !=
           contract.public_private_api_partition_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3PublicPrivateApiPartitionLoweringReplayKey(
    const Objc3PublicPrivateApiPartitionLoweringContract &contract) {
  return std::string("public_private_api_partition_sites=") +
         std::to_string(contract.public_private_api_partition_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3PublicPrivateApiPartitionLoweringLaneContract;
}

bool IsValidObjc3IncrementalModuleCacheInvalidationLoweringContract(
    const Objc3IncrementalModuleCacheInvalidationLoweringContract &contract) {
  if (contract.namespace_segment_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.import_edge_candidate_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.normalized_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.cache_invalidation_candidate_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.contract_violation_sites >
          contract.incremental_module_cache_invalidation_sites) {
    return false;
  }
  if (contract.normalized_sites +
          contract.cache_invalidation_candidate_sites >
      contract.incremental_module_cache_invalidation_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites !=
           contract.incremental_module_cache_invalidation_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3IncrementalModuleCacheInvalidationLoweringReplayKey(
    const Objc3IncrementalModuleCacheInvalidationLoweringContract &contract) {
  return std::string("incremental_module_cache_invalidation_sites=") +
         std::to_string(contract.incremental_module_cache_invalidation_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";cache_invalidation_candidate_sites=" +
         std::to_string(contract.cache_invalidation_candidate_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract;
}

bool IsValidObjc3CrossModuleConformanceLoweringContract(
    const Objc3CrossModuleConformanceLoweringContract &contract) {
  if (contract.namespace_segment_sites >
          contract.cross_module_conformance_sites ||
      contract.import_edge_candidate_sites >
          contract.cross_module_conformance_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites >
          contract.cross_module_conformance_sites ||
      contract.normalized_sites > contract.cross_module_conformance_sites ||
      contract.cache_invalidation_candidate_sites >
          contract.cross_module_conformance_sites ||
      contract.contract_violation_sites >
          contract.cross_module_conformance_sites) {
    return false;
  }
  if (contract.normalized_sites +
          contract.cache_invalidation_candidate_sites >
      contract.cross_module_conformance_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.cross_module_conformance_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3CrossModuleConformanceLoweringReplayKey(
    const Objc3CrossModuleConformanceLoweringContract &contract) {
  return std::string("cross_module_conformance_sites=") +
         std::to_string(contract.cross_module_conformance_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";cache_invalidation_candidate_sites=" +
         std::to_string(contract.cache_invalidation_candidate_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3CrossModuleConformanceLoweringLaneContract;
}
