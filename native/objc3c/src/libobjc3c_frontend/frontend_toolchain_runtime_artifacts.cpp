#include "libobjc3c_frontend/frontend_toolchain_runtime_artifacts.h"

#include "io/objc3_artifact_paths.h"
#include "io/objc3_process_runtime_metadata_contracts.h"
#include "libobjc3c_frontend/frontend_runtime_registration_artifact_inputs.h"
#include "libobjc3c_frontend/frontend_toolchain_runtime_sidecars.h"

namespace objc3c::frontend {

bool PublishFrontendToolchainRuntimeArtifacts(
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan,
    const std::filesystem::path &ir_out,
    const std::filesystem::path &object_out,
    const std::filesystem::path &backend_out,
    std::string &backend_output_error,
    std::string &backend_error) {
  Objc3RuntimeMetadataLinkerRetentionArtifacts linker_retention_artifacts;
  std::string linker_retention_error;
  if (!TryBuildObjc3RuntimeMetadataLinkerRetentionArtifacts(
          ir_out, object_out, linker_retention_artifacts,
          linker_retention_error)) {
    backend_error = linker_retention_error;
    return false;
  }

  if (!WriteBackendSidecar(
          BuildRuntimeMetadataLinkerResponseArtifactPath(
              artifact_plan.out_dir, artifact_plan.emit_prefix),
          linker_retention_artifacts.linker_response_file_payload,
          backend_output_error)) {
    return false;
  }
  if (!WriteBackendSidecar(
          BuildRuntimeMetadataDiscoveryArtifactPath(artifact_plan.out_dir,
                                                   artifact_plan.emit_prefix),
          linker_retention_artifacts.discovery_json, backend_output_error)) {
    return false;
  }

  if (!ValidateRuntimeRegistrationSummaries(product, backend_error)) {
    return false;
  }

  const Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs
      manifest_inputs =
          BuildRuntimeRegistrationManifestInputs(product, object_out, backend_out);
  std::string registration_manifest_json;
  std::string registration_manifest_error;
  if (!TryBuildObjc3RuntimeTranslationUnitRegistrationManifestArtifact(
          manifest_inputs, linker_retention_artifacts,
          product.artifact_bundle.runtime_metadata_binary.size(),
          registration_manifest_json, registration_manifest_error)) {
    backend_error = registration_manifest_error;
    return false;
  }
  if (!WriteBackendSidecar(
          BuildRuntimeRegistrationManifestArtifactPath(
              artifact_plan.out_dir, artifact_plan.emit_prefix),
          registration_manifest_json, backend_output_error)) {
    return false;
  }

  const Objc3RuntimeRegistrationDescriptorArtifactInputs descriptor_inputs =
      BuildRuntimeRegistrationDescriptorInputs(product, object_out, backend_out);
  std::string registration_descriptor_json;
  std::string registration_descriptor_error;
  if (!TryBuildObjc3RuntimeRegistrationDescriptorArtifact(
          descriptor_inputs, linker_retention_artifacts,
          registration_descriptor_json, registration_descriptor_error)) {
    backend_error = registration_descriptor_error;
    return false;
  }

  return WriteBackendSidecar(
      BuildRuntimeRegistrationDescriptorArtifactPath(artifact_plan.out_dir,
                                                     artifact_plan.emit_prefix),
      registration_descriptor_json, backend_output_error);
}

}  // namespace objc3c::frontend
