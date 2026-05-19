#pragma once

#include <iosfwd>

struct Objc3FrontendArtifactRuntimeMetadataPlan;
struct Objc3FrontendArtifactRuntimeRegistrationPlan;
struct Objc3FrontendPipelineResult;

namespace objc3::artifacts::frontend {

struct Objc3FrontendArtifactConformanceReportPlan;

void WriteObjc3FrontendRuntimeBootstrapManifestOrchestration(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendArtifactConformanceReportPlan &conformance_report_plan,
    const Objc3FrontendArtifactRuntimeMetadataPlan &runtime_metadata_plan,
    const Objc3FrontendArtifactRuntimeRegistrationPlan
        &runtime_registration_plan);

}  // namespace objc3::artifacts::frontend
