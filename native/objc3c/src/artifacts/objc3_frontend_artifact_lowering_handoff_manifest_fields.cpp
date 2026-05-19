#include "artifacts/objc3_frontend_artifact_lowering_handoff_manifest_fields.h"

#include "artifacts/objc3_frontend_artifact_lowering_handoff_block_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_lowering_handoff_core_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_lowering_handoff_error_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_lowering_handoff_manifest_field_modules.h"
#include "artifacts/objc3_frontend_artifact_lowering_handoff_ownership_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_lowering_handoff_semantic_summary_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_lowering_handoff_type_system_manifest_fields.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactLoweringHandoffManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactCoreLoweringPlan &core_lowering_plan,
    const Objc3FrontendArtifactOwnershipAwareLoweringPlan
        &ownership_aware_lowering_plan,
    const Objc3FrontendArtifactBlockLoweringPlan &block_lowering_plan,
    const Objc3FrontendArtifactTypeSystemLoweringPlan
        &type_system_lowering_plan,
    const Objc3FrontendArtifactRuntimeImportPlan &runtime_import_plan,
    const Objc3FrontendArtifactModuleLoweringPlan &module_lowering_plan,
    const Objc3FrontendArtifactErrorLoweringPlan &error_lowering_plan,
    const Objc3FrontendObjectPointerNullabilityGenericsSummary
        &object_pointer_nullability_generics_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary) {
  AppendObjc3FrontendArtifactLoweringHandoffCoreManifestFields(
      manifest, core_lowering_plan);
  AppendObjc3FrontendArtifactLoweringHandoffOwnershipManifestFields(
      manifest, ownership_aware_lowering_plan);
  AppendObjc3FrontendArtifactLoweringHandoffBlockManifestFields(
      manifest, block_lowering_plan);
  AppendObjc3FrontendArtifactLoweringHandoffTypeSystemManifestFields(
      manifest, type_system_lowering_plan);
  AppendObjc3FrontendArtifactLoweringHandoffRuntimeImportManifestFields(
      manifest, runtime_import_plan);
  AppendObjc3FrontendArtifactLoweringHandoffModuleManifestFields(
      manifest, module_lowering_plan);
  AppendObjc3FrontendArtifactLoweringHandoffErrorManifestFields(
      manifest, error_lowering_plan);
  AppendObjc3FrontendArtifactLoweringHandoffSemanticSummaryManifestFields(
      manifest,
      object_pointer_nullability_generics_summary,
      symbol_graph_scope_resolution_summary);
}

}  // namespace objc3::artifacts::frontend
