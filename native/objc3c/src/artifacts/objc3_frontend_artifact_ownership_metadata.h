#pragma once

#include <string>

#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"
#include "ir/objc3_ir_frontend_metadata.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendOwnershipMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &ownership_system_extension_lowering_replay_key,
    const Objc3OwnershipSystemExtensionLoweringContract
        &ownership_system_extension_lowering_contract,
    const std::string &ownership_borrowed_retainable_abi_completion_replay_key,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary
        &ownership_system_extension_source_closure_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &ownership_retainable_c_family_source_completion_summary);

}  // namespace objc3::artifacts::frontend
