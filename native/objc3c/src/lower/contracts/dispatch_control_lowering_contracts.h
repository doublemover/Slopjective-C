#pragma once

#include <cstddef>
#include <string>

#include "support/frontend_dispatch_contract_records.h"

// Dispatch-control lowering owns direct-call candidates, final/sealed dispatch
// intent, and the replayable interface-preservation contract built from it.
inline constexpr const char *kObjc3DispatchDispatchControlLoweringContractId =
    "objc3c.dispatch.dispatch.control.lowering.contract.v1";
inline constexpr const char *kObjc3DispatchDispatchControlLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_dispatch_dispatch_control_lowering_contract";
inline constexpr const char *kObjc3DispatchDispatchControlLoweringModel =
    "dispatch-direct-call-candidates-final-sealed-boundaries-and-dynamism-intent-metadata-now-feed-one-deterministic-lowering-contract-for-manifest-and-ir-carriage";
inline constexpr const char *kObjc3DispatchDispatchControlLoweringDeferredModel =
    "live-direct-call-selector-bypass-runtime-dispatch-boundary-realization-and-runnable-metadata-consumption-remain-later-dispatch-control-runtime-work";
inline constexpr const char *kObjc3DispatchDispatchControlLoweringLaneContract =
    "objc3c.dispatch.dispatch.control.lowering.contract.v1";

inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationContractId =
        "objc3c.dispatch.dispatch.metadata.interface.preservation.v1";
inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_dispatch_dispatch_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationImportArtifactMemberName =
        "objc_dispatch_dispatch_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationSourceModel =
        "runtime-metadata-source-records-and-runtime-import-surface-artifacts-preserve-direct-final-sealed-intent-for-separate-compilation-and-interface-replay";
inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationModel =
        "provider-and-consumer-runtime-import-surface-artifacts-preserve-direct-final-sealed-dispatch-intent-beyond-local-ir-object-emission";
inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationFailClosedModel =
        "missing-or-drifted-dispatch-intent-preservation-packets-disable-cross-module-dispatch-preservation-claims";
