#include "artifacts/objc3_frontend_artifact_runtime_bootstrap_manifest_fields.h"

#include <ostream>

#include "ast/objc3_ast_contracts_runtime_bootstrap_support_bootstrap_api.h"
#include "ast/objc3_ast_contracts_runtime_bootstrap_support_registrar_reset.h"
#include "io/objc3_json.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata_bootstrap_legality_surfaces.h"
#include "runtime/metadata/selector_metadata_bootstrap_surfaces.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeBootstrapManifestFields(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary
        &runtime_bootstrap_failure_restart_semantics,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering) {
#include "objc3_frontend_artifact_runtime_bootstrap_manifest_failure_restart_fields.inc"
#include "objc3_frontend_artifact_runtime_bootstrap_manifest_api_fields.inc"
#include "objc3_frontend_artifact_runtime_bootstrap_manifest_semantics_fields.inc"
#include "objc3_frontend_artifact_runtime_bootstrap_manifest_lowering_runtime_fields.inc"
}

}  // namespace objc3::artifacts::frontend
