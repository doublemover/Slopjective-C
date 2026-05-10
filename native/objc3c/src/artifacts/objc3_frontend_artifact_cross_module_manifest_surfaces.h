#pragma once

#include <iosfwd>
#include <string>

struct Objc3CrossModuleBuildRuntimeOrchestrationSummary;
struct Objc3CrossModuleConformanceLoweringContract;
struct Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary;
struct Objc3ImportedRuntimeMetadataSemanticRulesSummary;
struct Objc3IncrementalModuleCacheInvalidationLoweringContract;
struct Objc3ModuleImportGraphLoweringContract;
struct Objc3NamespaceCollisionShadowingLoweringContract;
struct Objc3ParserContractSnapshot;
struct Objc3Program;
struct Objc3PublicPrivateApiPartitionLoweringContract;
struct Objc3RuntimeAwareImportModuleFrontendClosureSummary;
struct Objc3SerializedRuntimeMetadataArtifactReuseSummary;
struct Objc3SerializedRuntimeMetadataImportLoweringSummary;

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
    const std::string &cross_module_conformance_lowering_replay_key);

}  // namespace objc3::artifacts::frontend
