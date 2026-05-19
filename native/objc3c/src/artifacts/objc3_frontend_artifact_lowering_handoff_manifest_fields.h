#pragma once

#include <sstream>

#include "artifacts/objc3_frontend_artifact_block_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_core_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_error_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_module_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_ownership_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_import_plan.h"
#include "artifacts/objc3_frontend_artifact_type_system_lowering_plan.h"
#include "sema/model/frontend_concurrency_symbol_graph_summaries.h"
#include "sema/model/frontend_linkage_summaries.h"

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
        &symbol_graph_scope_resolution_summary);

}  // namespace objc3::artifacts::frontend
