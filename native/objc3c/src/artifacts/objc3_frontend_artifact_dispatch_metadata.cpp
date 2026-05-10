#include "artifacts/objc3_frontend_artifact_dispatch_metadata.h"

#include "artifacts/objc3_frontend_artifact_dispatch_contract_snapshots.h"
#include "artifacts/objc3_frontend_artifact_dispatch_metadata_application.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendDispatchMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &dispatch_dispatch_control_lowering_replay_key,
    const Objc3DispatchDispatchControlLoweringContract
        &dispatch_dispatch_control_lowering_contract,
    const Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary
        &dispatch_dispatch_metadata_interface_preservation_summary) {
  ApplyObjc3FrontendDispatchMetadataSnapshots(
      ir_frontend_metadata,
      dispatch_dispatch_control_lowering_replay_key,
      BuildDispatchControlLoweringSnapshot(
          dispatch_dispatch_control_lowering_contract),
      BuildDispatchMetadataPreservationSnapshot(
          dispatch_dispatch_metadata_interface_preservation_summary));
}

}  // namespace objc3::artifacts::frontend
