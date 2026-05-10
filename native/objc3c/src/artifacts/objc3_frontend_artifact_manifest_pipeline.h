#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

#include "ast/objc3_ast.h"
#include "pipeline/objc3_frontend_types.h"
#include "sema/objc3_semantic_passes.h"
#include "support/objc3_runtime_metadata_record_set.h"

struct Objc3FrontendArtifactBundle;

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
    std::size_t vector_signature_functions,
    const std::string &property_synthesis_ivar_binding_replay_key,
    const Objc3PropertySynthesisIvarBindingContract
        &property_synthesis_ivar_binding_contract);

void AppendObjc3FrontendArtifactManifestRecordArrays(
    std::ostream &manifest,
    const Objc3Program &program,
    const std::vector<int> &resolved_global_values,
    const std::vector<const FunctionDecl *> &manifest_functions,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records);

}  // namespace objc3::artifacts::frontend
