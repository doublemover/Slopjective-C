#include "libobjc3c_frontend/frontend_runtime_registration_artifact_inputs.h"

#include "runtime/metadata/selector_metadata_registration_descriptor_surfaces.h"
#include "runtime/metadata/selector_metadata_registration_manifest.h"

namespace objc3c::frontend {

bool ValidateRuntimeRegistrationSummaries(
    const Objc3FrontendCompileProduct &product,
    std::string &backend_error) {
  if (!IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          product.artifact_bundle
              .runtime_translation_unit_registration_manifest_summary)) {
    backend_error = "translation-unit registration manifest template not ready";
    return false;
  }
  if (!IsReadyObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
          product.artifact_bundle
              .runtime_registration_descriptor_image_root_source_surface_summary)) {
    backend_error = "registration descriptor/image-root source surface not ready";
    return false;
  }
  if (!IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(
          product.artifact_bundle
              .runtime_registration_descriptor_frontend_closure_summary)) {
    backend_error = "registration descriptor frontend closure not ready";
    return false;
  }
  return true;
}

}  // namespace objc3c::frontend
