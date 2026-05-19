#include "artifacts/objc3_frontend_artifact_runtime_support_registration_manifest_fields.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_runtime_registration_plan.h"
#include "io/objc3_json.h"
#include "runtime/metadata/runtime_support_library_metadata.h"
#include "runtime/metadata/runtime_translation_unit_registration_metadata.h"
#include "runtime/metadata/selector_metadata_registration_manifest.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

#include "artifacts/objc3_frontend_artifact_runtime_support_registration_manifest_support_library_fields.inc"
#include "artifacts/objc3_frontend_artifact_runtime_support_registration_manifest_core_feature_fields.inc"
#include "artifacts/objc3_frontend_artifact_runtime_support_registration_manifest_link_wiring_fields.inc"
#include "artifacts/objc3_frontend_artifact_runtime_support_registration_manifest_translation_unit_contract_fields.inc"
#include "artifacts/objc3_frontend_artifact_runtime_support_registration_manifest_translation_unit_manifest_fields.inc"

}  // namespace

void AppendObjc3FrontendArtifactRuntimeSupportRegistrationManifestFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactRuntimeRegistrationPlan
        &runtime_registration_plan) {
  AppendObjc3RuntimeSupportLibraryManifestFields(
      manifest, runtime_registration_plan.runtime_support_library);
  AppendObjc3RuntimeSupportLibraryCoreFeatureManifestFields(
      manifest, runtime_registration_plan.runtime_support_library_core_feature);
  AppendObjc3RuntimeSupportLibraryLinkWiringManifestFields(
      manifest, runtime_registration_plan.runtime_support_library_link_wiring);
  AppendObjc3RuntimeSupportLibraryCoreFeatureFailureManifestField(
      manifest, runtime_registration_plan.runtime_support_library_core_feature);
  AppendObjc3RuntimeTranslationUnitRegistrationContractManifestFields(
      manifest,
      runtime_registration_plan.runtime_translation_unit_registration_contract);
  AppendObjc3RuntimeTranslationUnitRegistrationManifestFields(
      manifest,
      runtime_registration_plan.runtime_translation_unit_registration_manifest);
}

}  // namespace objc3::artifacts::frontend
