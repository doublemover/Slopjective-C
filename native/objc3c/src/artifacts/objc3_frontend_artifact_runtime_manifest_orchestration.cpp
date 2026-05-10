#include "artifacts/objc3_frontend_artifact_runtime_manifest_orchestration.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_conformance_report_plan.h"
#include "artifacts/objc3_frontend_artifact_executable_metadata_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_runtime_bootstrap_private_manifest_fields.h"
#include "artifacts/objc3_frontend_artifact_runtime_metadata_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_registration_plan.h"
#include "artifacts/objc3_frontend_runtime_bootstrap_artifacts.h"
#include "artifacts/objc3_frontend_runtime_descriptor_artifacts.h"
#include "artifacts/objc3_frontend_runtime_registration_artifacts.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

void WriteObjc3FrontendRuntimeBootstrapManifestOrchestration(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendArtifactConformanceReportPlan &conformance_report_plan,
    const Objc3FrontendArtifactRuntimeMetadataPlan &runtime_metadata_plan,
    const Objc3FrontendArtifactRuntimeRegistrationPlan
        &runtime_registration_plan) {
  WriteObjc3FrontendArtifactConformanceReportManifestSurfaces(
      manifest, conformance_report_plan);
  WriteExecutableRuntimeMetadataManifestSurfaces(
      manifest, pipeline_result.executable_metadata_source_graph,
      runtime_metadata_plan.runtime_metadata_source_to_section_matrix,
      pipeline_result.executable_metadata_semantic_consistency_boundary,
      pipeline_result.executable_metadata_semantic_validation_surface,
      pipeline_result.executable_metadata_lowering_handoff_surface,
      pipeline_result.executable_metadata_typed_lowering_handoff,
      runtime_metadata_plan.executable_metadata_debug_projection,
      runtime_metadata_plan.executable_metadata_runtime_ingest_packaging_contract,
      runtime_metadata_plan.executable_metadata_runtime_ingest_binary_boundary);
  manifest
           // translation-unit registration surface anchor: lane-A
           // freezes one manifest-published preregistration contract over the
           // runtime metadata binary, linker-retention sidecars, constructor
           // root reservation, and runtime-owned entrypoint boundary before
           // A002 emits any real startup constructor or bootstrap calls.
           << ",\"objc_runtime_translation_unit_registration_contract\":"
           << BuildRuntimeTranslationUnitRegistrationContractSummaryJson(
                  runtime_registration_plan
                      .runtime_translation_unit_registration_contract)
           // registration-manifest anchor: lane-A now publishes the
           // manifest template and constructor-root ownership model that later
           // lowering/bootstrap lanes consume directly instead of reconstructing
           // startup registration inputs ad hoc from loose sidecars.
           // registration-descriptor/image-root source-surface
           // anchor: the same semantic surface now freezes one canonical
           // frontend-visible naming model for the registration descriptor and
           // image root that later frontend closure, lowering, and runtime
           // replay work must preserve.
           // startup-registration gate anchor: the semantic-surface registration manifest remains the canonical lane-E gate input
           // for the A002/B002/C003/D003/D004 replay-stable bootstrap evidence chain.
           // runbook-closeout anchor: the registration manifest summary stays authoritative for the published runbook
           // and its live smoke replay proof.
           << ",\"objc_runtime_translation_unit_registration_manifest\":"
           << BuildRuntimeTranslationUnitRegistrationManifestSummaryJson(
                  runtime_registration_plan
                      .runtime_translation_unit_registration_manifest)
           << ",\"objc_runtime_registration_descriptor_image_root_source_surface\":"
           << BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceSummaryJson(
                  runtime_registration_plan
                      .runtime_registration_descriptor_image_root_source_surface)
           // registration-descriptor frontend-closure anchor: lane-A
           // now publishes the owned descriptor-artifact boundary that later
           // lowering and runtime bootstrap work consume directly.
           << ",\"objc_runtime_registration_descriptor_frontend_closure\":"
           << BuildRuntimeRegistrationDescriptorFrontendClosureSummaryJson(
                  runtime_registration_plan
                      .runtime_registration_descriptor_frontend_closure)
           // bootstrap-legality anchor: lane-B now publishes the
           // fail-closed semantic legality packet that bridges the emitted
           // descriptor frontier and the live bootstrap
           // semantics so later runtime/lowering work preserves one canonical
           // duplicate-policy, ordering, and restart model.
           << ",\"objc_runtime_bootstrap_legality_failure_contract\":"
           << BuildRuntimeBootstrapLegalityFailureContractSummaryJson(
                  runtime_registration_plan
                      .runtime_bootstrap_legality_failure_contract)
           // bootstrap-legality semantics anchor: lane-B now lands
           // the live duplicate-registration and image-order semantic bridge
           // over the emitted translation-unit identity key so lowering and
           // runtime handoff consume one canonical cross-image legality model.
           << ",\"objc_runtime_bootstrap_legality_semantics\":"
           << BuildRuntimeBootstrapLegalitySemanticsSummaryJson(
                  runtime_registration_plan.runtime_bootstrap_legality_semantics)
           // bootstrap failure/restart anchor: lane-B now publishes
           // the fail-closed restart/recovery bridge over the live reset/replay
           // runtime path so later multi-image bootstrap work consumes one
           // canonical unsupported-topology and deterministic-restart model.
           << ",\"objc_runtime_bootstrap_failure_restart_semantics\":"
           << BuildRuntimeBootstrapFailureRestartSemanticsSummaryJson(
                  runtime_registration_plan
                      .runtime_bootstrap_failure_restart_semantics)
           // runtime-bootstrap-api anchor: lane-D freezes the
           // runtime-owned bootstrap header/archive/entrypoint/reset surface as
           // one canonical packet that later image-walk and reset-expansion
           // issues must preserve exactly.
           << ",\"objc_runtime_bootstrap_api_contract\":"
           << BuildRuntimeBootstrapApiSummaryJson(
                  runtime_registration_plan.runtime_bootstrap_api);
  WriteRuntimeBootstrapPrivateManifestFields(
      manifest, runtime_registration_plan.runtime_bootstrap_api,
      runtime_registration_plan.runtime_bootstrap_lowering,
      runtime_registration_plan.runtime_bootstrap_failure_restart_semantics,
      runtime_registration_plan.runtime_registration_descriptor_frontend_closure,
      runtime_registration_plan.translation_unit_identity_key);
  manifest
           // bootstrap-invariant anchor: lane-B freezes duplicate
           // registration, realization order, failure mode, and image-local
           // initialization semantics against the live A002 registration
           // manifest so later bootstrap implementation extends one canonical
           // sema/runtime packet.
           << ",\"objc_runtime_startup_bootstrap_invariants\":"
           << BuildRuntimeStartupBootstrapInvariantSummaryJson(
                  runtime_registration_plan.runtime_startup_bootstrap_invariants)
           // bootstrap-semantics anchor: lane-B now lands the live
           // runtime enforcement/result-code surface that must remain aligned
           // with the emitted registration manifest and the native runtime
           // probe harness.
           << ",\"objc_runtime_startup_bootstrap_semantics\":"
           << BuildRuntimeBootstrapSemanticsSummaryJson(
                  runtime_registration_plan.runtime_bootstrap_semantics)
           // bootstrap-lowering anchor: lane-C now freezes one
           // manifest-driven lowering packet that owns future ctor-root,
           // init-stub, and registration-table materialization without
           // claiming that the current emitted IR already contains those
           // globals.
           << ",\"objc_runtime_bootstrap_lowering_contract\":"
           << BuildRuntimeBootstrapLoweringSummaryJson(
                  runtime_registration_plan.runtime_bootstrap_lowering);
}

}  // namespace objc3::artifacts::frontend
