#include "driver/objc3_driver_runtime_registration_publication.h"

#include <filesystem>
#include <string>

#include "driver/objc3_driver_diagnostic_output.h"
#include "driver/objc3_driver_runtime_registration_descriptor_inputs.h"
#include "driver/objc3_driver_runtime_registration_manifest_inputs.h"
#include "driver/objc3_driver_status_codes.h"
#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process.h"

Objc3DriverRuntimeRegistrationPublicationResult
PublishObjc3DriverRuntimeRegistrationArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts,
    const Objc3DriverObjectBackendResult &object_backend) {
  Objc3DriverRuntimeRegistrationPublicationResult result;
  result.compile_status = object_backend.compile_status;
  if (result.compile_status != 0) {
    return result;
  }

  std::string linker_retention_error;
  if (!TryBuildObjc3RuntimeMetadataLinkerRetentionArtifacts(
          object_backend.ir_out,
          object_backend.object_out,
          result.linker_retention_artifacts,
          linker_retention_error)) {
    result.compile_status = Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
    EmitObjc3DriverError(linker_retention_error);
    return result;
  }
  result.linker_retention_ready = true;

  WriteRuntimeMetadataLinkerResponseArtifact(
      cli_options.out_dir,
      cli_options.emit_prefix,
      result.linker_retention_artifacts.linker_response_file_payload);
  WriteRuntimeMetadataDiscoveryArtifact(
      cli_options.out_dir,
      cli_options.emit_prefix,
      result.linker_retention_artifacts.discovery_json);

  if (!IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          artifacts.runtime_translation_unit_registration_manifest_summary)) {
    result.compile_status = Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
    EmitObjc3DriverError(
        "translation-unit registration manifest template not ready");
    return result;
  }
  if (!IsReadyObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
          artifacts
              .runtime_registration_descriptor_image_root_source_surface_summary)) {
    result.compile_status = Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
    EmitObjc3DriverError(
        "registration descriptor/image-root source surface not ready");
    return result;
  }
  if (!IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(
          artifacts.runtime_registration_descriptor_frontend_closure_summary)) {
    result.compile_status = Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
    EmitObjc3DriverError(
        "registration descriptor frontend closure not ready");
    return result;
  }

  const Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs
      manifest_inputs = BuildObjc3DriverRuntimeRegistrationManifestInputs(
          artifacts,
          object_backend.object_out,
          object_backend.backend_out);
  std::string registration_manifest_json;
  std::string registration_manifest_error;
  if (!TryBuildObjc3RuntimeTranslationUnitRegistrationManifestArtifact(
          manifest_inputs,
          result.linker_retention_artifacts,
          artifacts.runtime_metadata_binary.size(),
          registration_manifest_json,
          registration_manifest_error)) {
    result.compile_status = Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
    EmitObjc3DriverError(registration_manifest_error);
    return result;
  }
  WriteRuntimeRegistrationManifestArtifact(
      cli_options.out_dir,
      cli_options.emit_prefix,
      registration_manifest_json);

  const Objc3RuntimeRegistrationDescriptorArtifactInputs descriptor_inputs =
      BuildObjc3DriverRuntimeRegistrationDescriptorInputs(
          artifacts,
          object_backend.object_out,
          object_backend.backend_out);
  std::string registration_descriptor_json;
  std::string registration_descriptor_error;
  if (!TryBuildObjc3RuntimeRegistrationDescriptorArtifact(
          descriptor_inputs,
          result.linker_retention_artifacts,
          registration_descriptor_json,
          registration_descriptor_error)) {
    result.compile_status = Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
    EmitObjc3DriverError(registration_descriptor_error);
    return result;
  }
  WriteRuntimeRegistrationDescriptorArtifact(
      cli_options.out_dir,
      cli_options.emit_prefix,
      registration_descriptor_json);
  return result;
}
