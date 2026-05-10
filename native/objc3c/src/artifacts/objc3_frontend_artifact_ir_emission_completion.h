#pragma once

#include "artifacts/objc3_frontend_artifacts.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/contracts/message_send_selector_lowering_contracts.h"
#include "lower/contracts/runtime_dispatch_abi_contracts.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendFinalRuntimeAndReadinessMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3FrontendArtifactBundle &bundle,
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    const Objc3RuntimeMetadataObjectInspectionHarnessSummary
        &runtime_metadata_object_inspection,
    const Objc3ExecutableMetadataDebugProjectionSummary
        &executable_metadata_debug_projection,
    const Objc3RuntimeSupportLibraryContractSummary &runtime_support_library,
    const Objc3RuntimeSupportLibraryCoreFeatureSummary
        &runtime_support_library_core_feature,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring,
    const Objc3OwnershipAwareLoweringBehaviorScaffold
        &ownership_aware_lowering_behavior_scaffold,
    const Objc3IREmissionCompletenessScaffold
        &ir_emission_completeness_scaffold,
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface
        &lowering_pipeline_pass_graph_core_feature_surface,
    const Objc3IREmissionCoreFeatureImplementationSurface
        &ir_emission_core_feature_impl_surface);

bool CompleteObjc3FrontendArtifactIREmission(
    Objc3FrontendArtifactBundle &bundle,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3Program &program,
    const Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3RuntimeDispatchLoweringAbiContract
        &runtime_dispatch_lowering_abi_contract,
    const Objc3MessageSendSelectorLoweringContract
        &message_send_selector_lowering_contract);

}  // namespace objc3::artifacts::frontend
