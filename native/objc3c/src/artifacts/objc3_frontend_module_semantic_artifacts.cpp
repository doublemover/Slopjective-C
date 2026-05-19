#include "artifacts/objc3_frontend_module_semantic_artifacts.h"

#include <algorithm>
#include <cstddef>

#include "lower/contracts/cross_module_lowering_contracts.h"
#include "sema/objc3_sema_parity_contract_surface.h"

namespace objc3::artifacts::frontend {

Objc3ModuleImportGraphLoweringContract BuildModuleImportGraphLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ModuleImportGraphLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.module_import_graph_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.module_import_graph_import_edge_candidate_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.module_import_graph_namespace_segment_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.module_import_graph_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.module_import_graph_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.module_import_graph_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.module_import_graph_contract_violation_sites_total;

  contract.module_import_graph_sites =
      std::max({raw_sites, raw_import_edge_sites, raw_namespace_segment_sites,
                raw_pointer_declarator_sites, raw_normalized_sites,
                raw_violation_sites});
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.module_import_graph_sites);
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.module_import_graph_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.module_import_graph_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.module_import_graph_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.module_import_graph_sites);
  contract.deterministic =
      sema_parity_surface.module_import_graph_summary.deterministic &&
      sema_parity_surface.deterministic_module_import_graph_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites == contract.module_import_graph_sites;
  return contract;
}

Objc3NamespaceCollisionShadowingLoweringContract
BuildNamespaceCollisionShadowingLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3NamespaceCollisionShadowingLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.namespace_collision_shadowing_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface
          .namespace_collision_shadowing_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface
          .namespace_collision_shadowing_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface
          .namespace_collision_shadowing_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface
          .namespace_collision_shadowing_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.namespace_collision_shadowing_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface
          .namespace_collision_shadowing_contract_violation_sites_total;

  contract.namespace_collision_shadowing_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites,
                raw_pointer_declarator_sites, raw_normalized_sites,
                raw_violation_sites});
  contract.namespace_segment_sites = std::min(
      raw_namespace_segment_sites, contract.namespace_collision_shadowing_sites);
  contract.import_edge_candidate_sites = std::min(
      raw_import_edge_sites, contract.namespace_collision_shadowing_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites = std::min(
      raw_pointer_declarator_sites,
      contract.namespace_collision_shadowing_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites,
               contract.namespace_collision_shadowing_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites,
               contract.namespace_collision_shadowing_sites);
  contract.deterministic =
      sema_parity_surface.namespace_collision_shadowing_summary.deterministic &&
      sema_parity_surface.deterministic_namespace_collision_shadowing_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites ==
          contract.namespace_collision_shadowing_sites;
  return contract;
}

Objc3PublicPrivateApiPartitionLoweringContract
BuildPublicPrivateApiPartitionLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3PublicPrivateApiPartitionLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.public_private_api_partition_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface
          .public_private_api_partition_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface
          .public_private_api_partition_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface
          .public_private_api_partition_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface
          .public_private_api_partition_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.public_private_api_partition_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface
          .public_private_api_partition_contract_violation_sites_total;

  contract.public_private_api_partition_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites,
                raw_pointer_declarator_sites, raw_normalized_sites,
                raw_violation_sites});
  contract.namespace_segment_sites = std::min(
      raw_namespace_segment_sites, contract.public_private_api_partition_sites);
  contract.import_edge_candidate_sites = std::min(
      raw_import_edge_sites, contract.public_private_api_partition_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites = std::min(
      raw_pointer_declarator_sites,
      contract.public_private_api_partition_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites,
               contract.public_private_api_partition_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites,
               contract.public_private_api_partition_sites);
  contract.deterministic =
      sema_parity_surface.public_private_api_partition_summary.deterministic &&
      sema_parity_surface.deterministic_public_private_api_partition_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites ==
          contract.public_private_api_partition_sites;
  return contract;
}

Objc3IncrementalModuleCacheInvalidationLoweringContract
BuildIncrementalModuleCacheInvalidationLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3IncrementalModuleCacheInvalidationLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.incremental_module_cache_invalidation_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface
          .incremental_module_cache_invalidation_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface
          .incremental_module_cache_invalidation_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface
          .incremental_module_cache_invalidation_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface
          .incremental_module_cache_invalidation_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface
          .incremental_module_cache_invalidation_normalized_sites_total;
  const std::size_t raw_cache_candidate_sites =
      sema_parity_surface
          .incremental_module_cache_invalidation_cache_invalidation_candidate_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface
          .incremental_module_cache_invalidation_contract_violation_sites_total;

  contract.incremental_module_cache_invalidation_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites,
                raw_pointer_declarator_sites, raw_normalized_sites,
                raw_cache_candidate_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites,
               contract.incremental_module_cache_invalidation_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites,
               contract.incremental_module_cache_invalidation_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites,
               contract.incremental_module_cache_invalidation_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites,
               contract.incremental_module_cache_invalidation_sites);
  const std::size_t normalized_budget =
      (contract.incremental_module_cache_invalidation_sites >=
       contract.normalized_sites)
          ? (contract.incremental_module_cache_invalidation_sites -
             contract.normalized_sites)
          : 0;
  contract.cache_invalidation_candidate_sites =
      std::min(raw_cache_candidate_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites,
               contract.incremental_module_cache_invalidation_sites);
  contract.deterministic =
      sema_parity_surface.incremental_module_cache_invalidation_summary
          .deterministic &&
      sema_parity_surface
          .deterministic_incremental_module_cache_invalidation_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites ==
          contract.incremental_module_cache_invalidation_sites;
  return contract;
}

Objc3CrossModuleConformanceLoweringContract
BuildCrossModuleConformanceLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3CrossModuleConformanceLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.cross_module_conformance_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.cross_module_conformance_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.cross_module_conformance_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.cross_module_conformance_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.cross_module_conformance_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.cross_module_conformance_normalized_sites_total;
  const std::size_t raw_cache_candidate_sites =
      sema_parity_surface
          .cross_module_conformance_cache_invalidation_candidate_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.cross_module_conformance_contract_violation_sites_total;

  contract.cross_module_conformance_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites,
                raw_pointer_declarator_sites, raw_normalized_sites,
                raw_cache_candidate_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites,
               contract.cross_module_conformance_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.cross_module_conformance_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites,
               contract.cross_module_conformance_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.cross_module_conformance_sites);
  const std::size_t normalized_budget =
      (contract.cross_module_conformance_sites >= contract.normalized_sites)
          ? (contract.cross_module_conformance_sites - contract.normalized_sites)
          : 0;
  contract.cache_invalidation_candidate_sites =
      std::min(raw_cache_candidate_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.cross_module_conformance_sites);
  contract.deterministic =
      sema_parity_surface.cross_module_conformance_summary.deterministic &&
      sema_parity_surface.deterministic_cross_module_conformance_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites == contract.cross_module_conformance_sites;
  return contract;
}

}  // namespace objc3::artifacts::frontend
