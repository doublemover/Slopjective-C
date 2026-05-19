#pragma once

#include <string>

#include "artifacts/objc3_frontend_artifact_metadata_dtos.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendModuleMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &module_import_graph_lowering_replay_key,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract,
    const std::string &namespace_collision_shadowing_lowering_replay_key,
    const Objc3NamespaceCollisionShadowingLoweringContract
        &namespace_collision_shadowing_lowering_contract,
    const std::string &public_private_api_partition_lowering_replay_key,
    const Objc3PublicPrivateApiPartitionLoweringContract
        &public_private_api_partition_lowering_contract,
    const std::string
        &incremental_module_cache_invalidation_lowering_replay_key,
    const Objc3IncrementalModuleCacheInvalidationLoweringContract
        &incremental_module_cache_invalidation_lowering_contract,
    const std::string &cross_module_conformance_lowering_replay_key,
    const Objc3CrossModuleConformanceLoweringContract
        &cross_module_conformance_lowering_contract);

}  // namespace objc3::artifacts::frontend
