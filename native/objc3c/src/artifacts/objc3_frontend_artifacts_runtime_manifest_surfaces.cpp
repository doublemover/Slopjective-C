#include "artifacts/objc3_frontend_artifacts_runtime_manifest_surfaces.h"

#include <ostream>
#include <string>

#include "artifacts/objc3_frontend_artifact_dispatch_accessor_manifest.h"
#include "artifacts/objc3_frontend_artifact_dispatch_accessor_manifest_contracts.h"
#include "artifacts/objc3_frontend_artifact_executable_accessor_layout_manifest.h"
#include "artifacts/objc3_frontend_artifact_property_atomicity_manifest.h"
#include "artifacts/objc3_frontend_artifact_runtime_block_manifest.h"
#include "artifacts/objc3_frontend_artifact_runtime_bootstrap_metadata.h"
#include "artifacts/objc3_frontend_artifact_runtime_concurrency_manifest.h"
#include "artifacts/objc3_frontend_artifact_runtime_object_manifest.h"
#include "artifacts/objc3_frontend_artifact_runtime_object_manifest_contracts.h"
#include "artifacts/objc3_frontend_artifact_runtime_release_manifest.h"
#include "artifacts/objc3_frontend_artifact_runtime_state_manifest.h"
#include "artifacts/objc3_frontend_artifact_storage_accessor_manifest_contracts.h"
#include "artifacts/objc3_frontend_artifact_storage_accessor_manifest.h"
#include "artifacts/objc3_runtime_state_publication_paths.h"
#include "runtime/metadata/class_metadata.h"
#include "runtime/metadata/property_metadata.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3::artifacts::frontend {

void WriteObjc3FrontendRuntimeManifestSurfaces(
    std::ostream &manifest,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    const Objc3ExecutableMetadataSourceGraph &executable_metadata_source_graph,
    const Objc3DispatchAndSynthesizedAccessorLoweringFields
        &dispatch_and_synthesized_accessor_lowering_fields,
    const Objc3DispatchAccessorRuntimeAbiFields
        &dispatch_accessor_runtime_abi_fields,
    const Objc3RuntimeDispatchTableReflectionRecordLoweringFields
        &runtime_dispatch_table_reflection_record_lowering_fields,
    const Objc3StorageAccessorRuntimeAbiFields
        &storage_accessor_runtime_abi_fields,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &runtime_registration_descriptor_image_root_source_surface,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering,
    const Objc3RuntimeBootstrapLegalitySemanticsSummary
        &runtime_bootstrap_legality_semantics,
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary
        &runtime_bootstrap_failure_restart_semantics) {
  const auto runtime_state_publication_paths =
      BuildRuntimeStatePublicationPaths(
          runtime_translation_unit_registration_manifest
              .manifest_artifact_relative_path);
  const std::string &runtime_state_publication_emit_prefix =
      runtime_state_publication_paths.emit_prefix;
  const auto accessor_storage_lowering_metadata_summary =
      BuildAccessorStorageLoweringMetadataSummary(runtime_metadata_source_records);
  const auto executable_accessor_layout_lowering_summary =
      BuildExecutableAccessorLayoutLoweringSummary(
          executable_metadata_source_graph);
  WriteDispatchAndSynthesizedAccessorLoweringSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      dispatch_and_synthesized_accessor_lowering_fields,
      accessor_storage_lowering_metadata_summary,
      runtime_metadata_section_publication);
  WriteDispatchAccessorRuntimeAbiSurface(
      manifest, dispatch_accessor_runtime_abi_fields);
  WriteStorageAccessorRuntimeAbiSurface(
      manifest, runtime_bootstrap_api, storage_accessor_runtime_abi_fields);
  WriteRuntimeStatePublicationSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest, runtime_bootstrap_semantics);
  WriteRuntimeBootstrapRegistrationSourceSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure,
      runtime_registration_descriptor_image_root_source_surface,
      runtime_bootstrap_lowering, runtime_bootstrap_legality_semantics);
  WriteRuntimeBootstrapLoweringRegistrationArtifactSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure,
      runtime_bootstrap_lowering, runtime_bootstrap_semantics);
  WriteRuntimeMultiImageStartupOrderingSourceSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure,
      runtime_bootstrap_legality_semantics,
      runtime_bootstrap_failure_restart_semantics, runtime_bootstrap_api,
      runtime_bootstrap_semantics);
  WriteRuntimeObjectModelRealizationSourceSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure,
      runtime_bootstrap_api);
  WriteRuntimePropertyIvarStorageAccessorSourceSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure);
  WriteRuntimeBlockArcUnifiedSourceSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure);
  WriteRuntimeOwnershipTransferCaptureFamilySourceSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure);
  WriteRuntimeBlockArcLoweringHelperSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure);
  WriteRuntimeBlockArcRuntimeAbiSurface(manifest, runtime_bootstrap_api);
  WriteRuntimePropertyIvarAccessorReflectionImplementationSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure,
      runtime_bootstrap_api);
  WriteExecutablePropertyAccessorLayoutLoweringSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      executable_accessor_layout_lowering_summary,
      runtime_metadata_section_publication);
  WriteExecutableIvarLayoutEmissionSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      executable_accessor_layout_lowering_summary,
      runtime_metadata_section_publication);
  WriteExecutableSynthesizedAccessorPropertyLoweringSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      executable_accessor_layout_lowering_summary,
      runtime_metadata_section_publication);
  WriteRuntimePropertyAtomicitySynthesisReflectionSourceSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure);
  WriteRuntimeRealizationLoweringReflectionArtifactSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure);
  WriteRuntimeDispatchTableReflectionRecordLoweringSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure,
      runtime_metadata_section_publication,
      runtime_dispatch_table_reflection_record_lowering_fields);
  WriteRuntimeObjectModelAbiQuerySurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure,
      runtime_bootstrap_api);
  WriteRuntimeUnifiedConcurrencySourceSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure);
  WriteRuntimeAsyncTaskActorNormalizationCompletionSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure);
  WriteRuntimeUnifiedConcurrencyLoweringMetadataSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure);
  WriteRuntimeUnifiedConcurrencyRuntimeAbiSurface(manifest, runtime_bootstrap_api);
  WriteRuntimeRealizationLookupReflectionImplementationSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure);
  WriteRuntimeReflectionQuerySurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure,
      runtime_bootstrap_api);
  WriteRuntimeRealizationLookupSemanticsSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure,
      runtime_bootstrap_api);
  WriteRuntimeClassMetaclassProtocolRealizationSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure,
      runtime_bootstrap_api);
  WriteRuntimeCategoryAttachmentMergedDispatchSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure,
      runtime_bootstrap_api);
  WriteRuntimeReflectionVisibilityCoherenceDiagnosticsSurface(
      manifest, runtime_state_publication_emit_prefix,
      runtime_translation_unit_registration_manifest,
      runtime_registration_descriptor_frontend_closure,
      runtime_bootstrap_api);
  WriteRuntimeInstallationAbiSurface(manifest, runtime_bootstrap_api);
  WriteRuntimeLoaderLifecycleSurface(manifest, runtime_bootstrap_semantics);
  WriteRuntimeReleaseCandidateClaimAbiSurface(manifest, runtime_bootstrap_api);
  WriteRuntimeFinalReleaseEvidenceDescaffoldingImplementationSurface(
      manifest, runtime_bootstrap_api);
}

}  // namespace objc3::artifacts::frontend
