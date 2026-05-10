#pragma once

#include <cstddef>
#include <string>

struct Objc3ExecutableMetadataDebugProjectionSummary;
struct Objc3ExecutableMetadataTypedLoweringHandoff;
struct Objc3FrontendArtifactBundle;
struct Objc3FrontendOptions;
struct Objc3FrontendPipelineResult;
struct Objc3IREmissionCompletenessScaffold;
struct Objc3IREmissionCoreFeatureImplementationSurface;
struct Objc3IRFrontendMetadata;
struct Objc3LoweringPipelinePassGraphCoreFeatureSurface;
struct Objc3OwnershipAwareLoweringBehaviorScaffold;
struct Objc3Program;
struct Objc3RuntimeMetadataObjectInspectionHarnessSummary;
struct Objc3RuntimeMetadataSectionPublicationSummary;
struct Objc3RuntimeSupportLibraryContractSummary;
struct Objc3RuntimeSupportLibraryCoreFeatureSummary;
struct Objc3RuntimeSupportLibraryLinkWiringSummary;

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
    const std::string &runtime_dispatch_lowering_abi_boundary_summary,
    std::size_t message_send_sites);

}  // namespace objc3::artifacts::frontend
