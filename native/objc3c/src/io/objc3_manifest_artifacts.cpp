#include "io/objc3_manifest_artifacts.h"

#include <string>

#include "ast/objc3_ast.h"
#include "io/objc3_file_io.h"
#include "lower/objc3_lowering_contract.h"
#include "pipeline/objc3_frontend_types.h"

std::filesystem::path BuildManifestArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  // M266-E001 control-flow execution gate anchor: lane-E consumes the emitted
  // manifest plus the matching IR/object artifacts as one canonical proof
  // surface for the currently runnable Part 5 control-flow slice.
  // M266-E002 runnable control-flow matrix anchor: the closeout matrix keeps
  // consuming this same manifest artifact rather than inventing a separate
  // publication channel for Part 5 runnable evidence.
  // M265-E001 type-surface executable gate anchor: the emitted manifest
  // remains the authoritative published semantic packet surface consumed by
  // the lane-E executable Part 3 gate.
  // M265-E002 runnable-type-surface closeout anchor: closeout matrix rows
  // consume the same manifest surface for truthful optionals/generic-replay/
  // typed-keypath evidence.
  // M267-E001 error-model conformance gate anchor: lane-E keeps consuming the
  // same manifest sidecar and the canonical cross-module link-plan artifact
  // instead of introducing a parallel Part 6 publication surface.
  // M268-E001 async executable conformance gate anchor: lane-E keeps consuming
  // the same manifest sidecar and its paired IR/object artifacts instead of a
  // separate Part 7 publication surface.
  // M268-E002 runnable async closeout matrix anchor: the milestone closeout
  // keeps consuming this same manifest sidecar and its paired IR/object
  // artifacts instead of a matrix-only reporting surface.
  // M269-E001 task/executor conformance gate anchor: lane-E keeps consuming
  // the same manifest sidecar and paired driver artifacts while the wider
  // front-door task publication path remains intentionally fail-closed.
  // M269-E002 runnable task/executor closeout matrix anchor: the milestone
  // closeout keeps consuming this same manifest sidecar instead of inventing a
  // matrix-only reporting surface for the current Part 7 task/runtime slice.
  // M270-E001 strict concurrency conformance gate anchor: lane-E keeps
  // consuming the same manifest sidecar and paired driver artifacts while the
  // broader front-door actor publication path remains intentionally fail-closed.
  // M270-E002 runnable actor/isolation closeout matrix anchor: the milestone
  // closeout keeps consuming this same manifest sidecar instead of inventing a
  // matrix-only reporting surface for the current Part 7 actor/runtime slice.
  // M271-E001 strict system conformance gate anchor: lane-E keeps consuming
  // this same manifest sidecar and paired driver artifacts while the broader
  // front-door Part 8 publication path remains intentionally fail-closed for
  // deferred borrowed-lifetime/runtime-enforcement claims.
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

std::filesystem::path BuildPart6ResultBridgeArtifactReplayPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {

  return out_dir /
         (emit_prefix +
          kObjc3Part6ResultAndBridgingArtifactReplayArtifactSuffix);
}

std::filesystem::path BuildVersionedConformanceReportArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir /
         (emit_prefix +
          kObjc3VersionedConformanceReportLoweringArtifactSuffix);
}

std::filesystem::path BuildConformancePublicationArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + ".objc3-conformance-publication.json");
}

std::filesystem::path BuildConformanceValidationArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  // M264-E001 versioning/conformance truth-gate anchor: the validation
  // artifact is the canonical integrated operator-side summary of the emitted
  // report/publication pair.
  // M264-E002 release/runtime-claim-matrix anchor: the release matrix consumes
  // this stable validation artifact name when publishing the milestone closeout
  // surface.
  return out_dir / (emit_prefix + ".objc3-conformance-validation.json");
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

void WritePart6ResultBridgeArtifactReplay(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &artifact_json) {
  WriteText(BuildPart6ResultBridgeArtifactReplayPath(out_dir, emit_prefix),
            artifact_json);
}

void WriteVersionedConformanceReportArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &artifact_json) {
  WriteText(BuildVersionedConformanceReportArtifactPath(out_dir, emit_prefix),
            artifact_json);
}

void WriteConformancePublicationArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &artifact_json) {
  WriteText(BuildConformancePublicationArtifactPath(out_dir, emit_prefix),
            artifact_json);
}

void WriteConformanceValidationArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &artifact_json) {
  WriteText(BuildConformanceValidationArtifactPath(out_dir, emit_prefix),
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
