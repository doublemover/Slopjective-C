#pragma once

#include <string>

#include "artifacts/objc3_frontend_interop_semantic_artifacts.h"
#include "ir/objc3_ir_frontend_metadata.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendInteropMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &interop_interop_lowering_replay_key,
    const Objc3InteropInteropLoweringContract &interop_interop_lowering_contract,
    const std::string &interop_foreign_call_lifetime_lowering_replay_key,
    const Objc3InteropForeignCallLifetimeLoweringContract
        &interop_foreign_call_lifetime_lowering_contract,
    const std::string &interop_ffi_metadata_interface_preservation_replay_key,
    const Objc3InteropFfiMetadataInterfacePreservationContract
        &interop_ffi_metadata_interface_preservation_contract,
    const Objc3InteropHeaderModuleBridgeGenerationSummary
        &interop_header_module_bridge_generation_summary);

}  // namespace objc3::artifacts::frontend
