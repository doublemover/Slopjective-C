#pragma once

#include "artifacts/objc3_frontend_artifact_lowering_handoff_manifest_fields.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactLoweringHandoffCoreManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactCoreLoweringPlan &core_lowering_plan);

void AppendObjc3FrontendArtifactLoweringHandoffOwnershipManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactOwnershipAwareLoweringPlan
        &ownership_aware_lowering_plan);

void AppendObjc3FrontendArtifactLoweringHandoffTypeSystemManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactTypeSystemLoweringPlan
        &type_system_lowering_plan);

void AppendObjc3FrontendArtifactLoweringHandoffRuntimeImportManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactRuntimeImportPlan &runtime_import_plan);

void AppendObjc3FrontendArtifactLoweringHandoffModuleManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactModuleLoweringPlan &module_lowering_plan);

void AppendObjc3FrontendArtifactLoweringHandoffErrorManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactErrorLoweringPlan &error_lowering_plan);

void AppendObjc3FrontendArtifactLoweringHandoffSemanticSummaryManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendObjectPointerNullabilityGenericsSummary
        &object_pointer_nullability_generics_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary);

}  // namespace objc3::artifacts::frontend
