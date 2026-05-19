#pragma once

#include <iosfwd>
#include <string>

#include "ast/objc3_ast_declarations.h"
#include "lower/contracts/cross_module_lowering_contracts.h"
#include "parse/objc3_parser_contract_types.h"
#include "pipeline/results/runtime_import_evidence_record.h"
#include "sema/model/semantic_type_cross_module_build_orchestration.h"
#include "sema/model/semantic_type_cross_module_runtime_preservation.h"
#include "sema/model/semantic_type_imported_runtime_metadata_rules.h"
#include "sema/model/semantic_type_serialized_runtime_metadata.h"

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
