#include "io/objc3_manifest_artifacts.h"

#include <string>

#include "ast/objc3_ast.h"
#include "io/objc3_file_io.h"
#include "lower/objc3_lowering_contract.h"
#include "pipeline/objc3_frontend_types.h"

std::filesystem::path BuildManifestArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + ".manifest.json");
}

std::filesystem::path BuildRuntimeMetadataBinaryArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir /
         (emit_prefix + kObjc3ExecutableMetadataRuntimeIngestBinaryArtifactSuffix);
}

std::filesystem::path BuildRuntimeMetadataLinkerResponseArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + kObjc3RuntimeLinkerResponseArtifactSuffix);
}

std::filesystem::path BuildRuntimeMetadataDiscoveryArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + kObjc3RuntimeLinkerDiscoveryArtifactSuffix);
}

std::filesystem::path BuildRuntimeAwareImportModuleArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir /
         (emit_prefix +
          kObjc3RuntimeAwareImportModuleFrontendClosureArtifactSuffix);
}

std::filesystem::path BuildRuntimeRegistrationManifestArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  // M263-E001 bootstrap-completion gate anchor: the registration manifest is
  // one half of the canonical emitted artifact pair consumed by the lane-E bootstrap completion gate
  // for single-image and multi-image evidence.
  // M263-E002 bootstrap-matrix closeout anchor: published bootstrap matrix consumes the same canonical artifact pair.
  return out_dir /
         (emit_prefix +
          kObjc3RuntimeTranslationUnitRegistrationManifestArtifactSuffix);
}

std::filesystem::path BuildRuntimeRegistrationDescriptorArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  // M263-E001 bootstrap-completion gate anchor: the registration descriptor is
  // the other half of the canonical emitted artifact pair consumed by the
  // lane-E bootstrap completion gate.
  // M263-E002 bootstrap-matrix closeout anchor: published bootstrap matrix consumes the same canonical artifact pair.
  return out_dir /
         (emit_prefix +
          kObjc3RuntimeRegistrationDescriptorFrontendClosureArtifactSuffix);
}

std::filesystem::path BuildCrossModuleRuntimeLinkPlanArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + kObjc3CrossModuleRuntimeLinkPlanArtifactSuffix);
}

std::filesystem::path BuildCrossModuleRuntimeLinkerResponseArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir /
         (emit_prefix + kObjc3CrossModuleRuntimeLinkerResponseArtifactSuffix);
}

void WriteManifestArtifact(const std::filesystem::path &out_dir,
                           const std::string &emit_prefix,
                           const std::string &manifest_json) {
  WriteText(BuildManifestArtifactPath(out_dir, emit_prefix), manifest_json);
}

void WriteRuntimeMetadataBinaryArtifact(const std::filesystem::path &out_dir,
                                        const std::string &emit_prefix,
                                        const std::string &binary_payload) {
  WriteBytes(BuildRuntimeMetadataBinaryArtifactPath(out_dir, emit_prefix),
             binary_payload);
}

void WriteRuntimeMetadataLinkerResponseArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &response_payload) {
  WriteText(BuildRuntimeMetadataLinkerResponseArtifactPath(out_dir, emit_prefix),
            response_payload);
}

void WriteRuntimeMetadataDiscoveryArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &discovery_json) {
  WriteText(BuildRuntimeMetadataDiscoveryArtifactPath(out_dir, emit_prefix),
            discovery_json);
}

void WriteRuntimeAwareImportModuleArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &artifact_json) {
  WriteText(BuildRuntimeAwareImportModuleArtifactPath(out_dir, emit_prefix),
            artifact_json);
}

void WriteRuntimeRegistrationManifestArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &manifest_json) {
  WriteText(BuildRuntimeRegistrationManifestArtifactPath(out_dir, emit_prefix),
            manifest_json);
}

void WriteRuntimeRegistrationDescriptorArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &descriptor_json) {
  WriteText(BuildRuntimeRegistrationDescriptorArtifactPath(out_dir, emit_prefix),
            descriptor_json);
}

void WriteCrossModuleRuntimeLinkPlanArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &plan_json) {
  WriteText(BuildCrossModuleRuntimeLinkPlanArtifactPath(out_dir, emit_prefix),
            plan_json);
}

void WriteCrossModuleRuntimeLinkerResponseArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &response_payload) {
  WriteText(
      BuildCrossModuleRuntimeLinkerResponseArtifactPath(out_dir, emit_prefix),
      response_payload);
}
