#include "artifacts/objc3_frontend_artifact_cross_module_manifest_surfaces.h"

#include <ostream>

#include "artifacts/objc3_frontend_module_semantic_artifacts.h"
#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

namespace objc3::artifacts::frontend {

void WriteCrossModuleManifestSurfaces(
    std::ostream &manifest,
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract,
    const std::string &module_import_graph_lowering_replay_key,
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary
        &runtime_aware_import_module_frontend_closure,
    const Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary
        &cross_module_runtime_metadata_semantic_preservation,
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary
        &imported_runtime_metadata_semantic_rules,
    const Objc3SerializedRuntimeMetadataImportLoweringSummary
        &serialized_runtime_metadata_import_lowering,
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary
        &serialized_runtime_metadata_artifact_reuse,
    const Objc3CrossModuleBuildRuntimeOrchestrationSummary
        &cross_module_build_runtime_orchestration,
    const Objc3NamespaceCollisionShadowingLoweringContract
        &namespace_collision_shadowing_lowering_contract,
    const std::string &namespace_collision_shadowing_lowering_replay_key,
    const Objc3PublicPrivateApiPartitionLoweringContract
        &public_private_api_partition_lowering_contract,
    const std::string &public_private_api_partition_lowering_replay_key,
    const Objc3IncrementalModuleCacheInvalidationLoweringContract
        &incremental_module_cache_invalidation_lowering_contract,
    const std::string
        &incremental_module_cache_invalidation_lowering_replay_key,
    const Objc3CrossModuleConformanceLoweringContract
        &cross_module_conformance_lowering_contract,
    const std::string &cross_module_conformance_lowering_replay_key) {
  manifest
      << ",\"objc_module_import_graph_lowering_surface\":{\"module_import_graph_sites\":"
      << module_import_graph_lowering_contract.module_import_graph_sites
      << ",\"import_edge_candidate_sites\":"
      << module_import_graph_lowering_contract.import_edge_candidate_sites
      << ",\"namespace_segment_sites\":"
      << module_import_graph_lowering_contract.namespace_segment_sites
      << ",\"object_pointer_type_sites\":"
      << module_import_graph_lowering_contract.object_pointer_type_sites
      << ",\"pointer_declarator_sites\":"
      << module_import_graph_lowering_contract.pointer_declarator_sites
      << ",\"normalized_sites\":"
      << module_import_graph_lowering_contract.normalized_sites
      << ",\"contract_violation_sites\":"
      << module_import_graph_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << module_import_graph_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (module_import_graph_lowering_contract.deterministic ? "true"
                                                              : "false")
      << "}"
      // runtime-aware import/module surface anchor: lane-A freezes one
      // frontend-published contract above the current module import graph
      // lowering surface and below any real imported runtime-owned declaration
      // or metadata-reference realization.
      << ",\"objc_runtime_aware_import_module_surface_contract\":"
      << BuildRuntimeAwareImportModuleSurfaceSummaryJson(
             program, parser_contract_snapshot,
             module_import_graph_lowering_contract)
      << ",\"objc_runtime_aware_import_module_frontend_closure\":"
      << BuildRuntimeAwareImportModuleFrontendClosureSummaryJson(
             runtime_aware_import_module_frontend_closure)
      // cross-module semantic preservation anchor: lane-B freezes the semantic
      // facts that later imported metadata handling must preserve across module
      // boundaries without claiming that imported runtime metadata semantics
      // are landed yet.
      << ",\"objc_cross_module_runtime_metadata_semantic_preservation_contract\":"
      << BuildCrossModuleRuntimeMetadataSemanticPreservationSummaryJson(
             cross_module_runtime_metadata_semantic_preservation)
      << ",\"objc_imported_runtime_metadata_semantic_rules\":"
      << BuildImportedRuntimeMetadataSemanticRulesSummaryJson(
             imported_runtime_metadata_semantic_rules)
      // serialized metadata import/lowering anchor: lane-C freezes the boundary
      // where emitted runtime-import-surface artifacts already influence
      // frontend semantic surfaces, but imported metadata payloads still are not
      // rehydrated, reused incrementally, or lowered into IR.
      << ",\"objc_serialized_runtime_metadata_import_lowering_contract\":"
      << BuildSerializedRuntimeMetadataImportLoweringSummaryJson(
             serialized_runtime_metadata_import_lowering)
      // serialized metadata artifact reuse anchor: lane-C now emits and reloads
      // a transitive serialized runtime-metadata payload through the
      // runtime-import-surface artifact so downstream modules can recover
      // object-model semantics without reparsing source.
      << ",\"objc_serialized_runtime_metadata_artifact_reuse\":"
      << BuildSerializedRuntimeMetadataArtifactReuseSummaryJson(
             serialized_runtime_metadata_artifact_reuse)
      // cross-module build/runtime orchestration anchor: lane-D freezes the
      // truthful boundary where the transitive runtime-import-surface reuse
      // payload and the local registration manifest are both authoritative
      // inputs, while cross-module link packaging and runtime-registration
      // aggregation remain unlanded.
      // cross-module runtime packaging anchor: driver/runtime packaging path now
      // materializes the ordered cross-module link plan and merged linker
      // response file from those same authoritative artifacts, so this semantic
      // surface remains the canonical replay boundary for downstream packaging
      // consumers.
      // cross-module object-model gate anchor: lane-E consumes the
      // A002/B002/C002/D002 summary chain and freezes the current runnable
      // two-image proof boundary before E002 broadens the execution matrix.
      // runnable import/module execution-matrix anchor: the same emitted
      // frontend surface remains the canonical replay boundary while lane-E
      // closes the runnable matrix around it.
      << ",\"objc_cross_module_build_runtime_orchestration_contract\":"
      << BuildCrossModuleBuildRuntimeOrchestrationSummaryJson(
             cross_module_build_runtime_orchestration)
      << ",\"objc_namespace_collision_shadowing_lowering_surface\":{\"namespace_collision_shadowing_sites\":"
      << namespace_collision_shadowing_lowering_contract
             .namespace_collision_shadowing_sites
      << ",\"namespace_segment_sites\":"
      << namespace_collision_shadowing_lowering_contract.namespace_segment_sites
      << ",\"import_edge_candidate_sites\":"
      << namespace_collision_shadowing_lowering_contract
             .import_edge_candidate_sites
      << ",\"object_pointer_type_sites\":"
      << namespace_collision_shadowing_lowering_contract.object_pointer_type_sites
      << ",\"pointer_declarator_sites\":"
      << namespace_collision_shadowing_lowering_contract.pointer_declarator_sites
      << ",\"normalized_sites\":"
      << namespace_collision_shadowing_lowering_contract.normalized_sites
      << ",\"contract_violation_sites\":"
      << namespace_collision_shadowing_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << namespace_collision_shadowing_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (namespace_collision_shadowing_lowering_contract.deterministic
              ? "true"
              : "false")
      << "}"
      << ",\"objc_public_private_api_partition_lowering_surface\":{\"public_private_api_partition_sites\":"
      << public_private_api_partition_lowering_contract
             .public_private_api_partition_sites
      << ",\"namespace_segment_sites\":"
      << public_private_api_partition_lowering_contract.namespace_segment_sites
      << ",\"import_edge_candidate_sites\":"
      << public_private_api_partition_lowering_contract
             .import_edge_candidate_sites
      << ",\"object_pointer_type_sites\":"
      << public_private_api_partition_lowering_contract.object_pointer_type_sites
      << ",\"pointer_declarator_sites\":"
      << public_private_api_partition_lowering_contract.pointer_declarator_sites
      << ",\"normalized_sites\":"
      << public_private_api_partition_lowering_contract.normalized_sites
      << ",\"contract_violation_sites\":"
      << public_private_api_partition_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << public_private_api_partition_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (public_private_api_partition_lowering_contract.deterministic ? "true"
                                                                       : "false")
      << "}"
      << ",\"objc_incremental_module_cache_invalidation_lowering_surface\":{\"incremental_module_cache_invalidation_sites\":"
      << incremental_module_cache_invalidation_lowering_contract
             .incremental_module_cache_invalidation_sites
      << ",\"namespace_segment_sites\":"
      << incremental_module_cache_invalidation_lowering_contract
             .namespace_segment_sites
      << ",\"import_edge_candidate_sites\":"
      << incremental_module_cache_invalidation_lowering_contract
             .import_edge_candidate_sites
      << ",\"object_pointer_type_sites\":"
      << incremental_module_cache_invalidation_lowering_contract
             .object_pointer_type_sites
      << ",\"pointer_declarator_sites\":"
      << incremental_module_cache_invalidation_lowering_contract
             .pointer_declarator_sites
      << ",\"normalized_sites\":"
      << incremental_module_cache_invalidation_lowering_contract.normalized_sites
      << ",\"cache_invalidation_candidate_sites\":"
      << incremental_module_cache_invalidation_lowering_contract
             .cache_invalidation_candidate_sites
      << ",\"contract_violation_sites\":"
      << incremental_module_cache_invalidation_lowering_contract
             .contract_violation_sites
      << ",\"replay_key\":\""
      << incremental_module_cache_invalidation_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (incremental_module_cache_invalidation_lowering_contract.deterministic
              ? "true"
              : "false")
      << "}"
      << ",\"objc_cross_module_conformance_lowering_surface\":{\"cross_module_conformance_sites\":"
      << cross_module_conformance_lowering_contract.cross_module_conformance_sites
      << ",\"namespace_segment_sites\":"
      << cross_module_conformance_lowering_contract.namespace_segment_sites
      << ",\"import_edge_candidate_sites\":"
      << cross_module_conformance_lowering_contract.import_edge_candidate_sites
      << ",\"object_pointer_type_sites\":"
      << cross_module_conformance_lowering_contract.object_pointer_type_sites
      << ",\"pointer_declarator_sites\":"
      << cross_module_conformance_lowering_contract.pointer_declarator_sites
      << ",\"normalized_sites\":"
      << cross_module_conformance_lowering_contract.normalized_sites
      << ",\"cache_invalidation_candidate_sites\":"
      << cross_module_conformance_lowering_contract
             .cache_invalidation_candidate_sites
      << ",\"contract_violation_sites\":"
      << cross_module_conformance_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << cross_module_conformance_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (cross_module_conformance_lowering_contract.deterministic ? "true"
                                                                   : "false")
      << "}";
}

}  // namespace objc3::artifacts::frontend
