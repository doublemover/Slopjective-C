#include "io/objc3_artifact_paths.h"

#include "ast/objc3_ast.h"
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

std::filesystem::path BuildErrorHandlingResultBridgeArtifactReplayPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir /
         (emit_prefix +
          kObjc3ErrorHandlingResultAndBridgingArtifactReplayArtifactSuffix);
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
  return out_dir / (emit_prefix + ".objc3-conformance-validation.json");
}

std::filesystem::path BuildReleaseEvidenceOperationArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + ".objc3-release-evidence-operation.json");
}

std::filesystem::path BuildDashboardStatusArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + ".objc3-dashboard-status.json");
}

std::filesystem::path BuildAdvancedFeatureGateArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + ".objc3-advanced-feature-gate.json");
}

std::filesystem::path BuildReleaseCandidateMatrixArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + ".objc3-release-candidate-matrix.json");
}

std::filesystem::path BuildRuntimeRegistrationManifestArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir /
         (emit_prefix +
          kObjc3RuntimeTranslationUnitRegistrationManifestArtifactSuffix);
}

std::filesystem::path BuildRuntimeRegistrationDescriptorArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
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

std::filesystem::path BuildMetaprogrammingMacroHostProcessCacheArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir /
         (emit_prefix + kObjc3MetaprogrammingMacroHostProcessCacheArtifactSuffix);
}

std::filesystem::path BuildInteropBridgeHeaderArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + kObjc3InteropBridgeHeaderArtifactSuffix);
}

std::filesystem::path BuildInteropBridgeModuleArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + kObjc3InteropBridgeModuleArtifactSuffix);
}

std::filesystem::path BuildInteropBridgeArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + kObjc3InteropBridgeArtifactSuffix);
}
