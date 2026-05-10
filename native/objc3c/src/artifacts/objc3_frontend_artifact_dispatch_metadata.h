#pragma once

#include <string>

struct Objc3DispatchDispatchControlLoweringContract;
struct Objc3IRFrontendMetadata;

namespace objc3::artifacts::frontend {

struct Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary;

void ApplyObjc3FrontendDispatchMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &dispatch_dispatch_control_lowering_replay_key,
    const Objc3DispatchDispatchControlLoweringContract
        &dispatch_dispatch_control_lowering_contract,
    const Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary
        &dispatch_dispatch_metadata_interface_preservation_summary);

}  // namespace objc3::artifacts::frontend
