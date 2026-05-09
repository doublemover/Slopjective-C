#pragma once

#include <string>

#include "artifacts/objc3_frontend_runtime_import_artifacts.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/contracts/dispatch_control_lowering_contracts.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendDispatchMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &dispatch_dispatch_control_lowering_replay_key,
    const Objc3DispatchDispatchControlLoweringContract
        &dispatch_dispatch_control_lowering_contract,
    const Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary
        &dispatch_dispatch_metadata_interface_preservation_summary);

}  // namespace objc3::artifacts::frontend
