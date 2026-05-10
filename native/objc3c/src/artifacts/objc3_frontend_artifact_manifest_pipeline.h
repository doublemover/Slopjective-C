#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_manifest_pipeline_contracts.h"

struct FunctionDecl;
struct Objc3FrontendArtifactBundle;
struct Objc3FrontendOptions;
struct Objc3FrontendPipelineResult;
struct Objc3Program;
struct Objc3RuntimeMetadataSourceRecordSet;
struct Objc3SemanticTypeMetadataHandoff;

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactManifestPipelineStages(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3FrontendArtifactBundle &bundle);

void AppendObjc3FrontendArtifactManifestSemaPassDiagnostics(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result);

void AppendObjc3FrontendArtifactManifestLoweringHeader(
    std::ostream &manifest,
    const Objc3FrontendOptions &options,
    const Objc3FrontendArtifactManifestLoweringHeaderFields
        &lowering_header_fields);

void AppendObjc3FrontendArtifactManifestRecordArrays(
    std::ostream &manifest,
    const Objc3Program &program,
    const std::vector<int> &resolved_global_values,
    const std::vector<const FunctionDecl *> &manifest_functions,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records);

}  // namespace objc3::artifacts::frontend
