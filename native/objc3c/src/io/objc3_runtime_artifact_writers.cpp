#include "io/objc3_artifact_writers.h"

#include "io/objc3_artifact_paths.h"
#include "io/objc3_file_io.h"

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

void WriteStandaloneTextualInterfacePayloadArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &artifact_json) {
  WriteText(BuildStandaloneTextualInterfacePayloadArtifactPath(out_dir,
                                                               emit_prefix),
            artifact_json);
}

void WriteErrorHandlingResultBridgeArtifactReplay(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &artifact_json) {
  WriteText(BuildErrorHandlingResultBridgeArtifactReplayPath(out_dir, emit_prefix),
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

void WriteMetaprogrammingMacroHostProcessCacheArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &artifact_json) {
  WriteText(BuildMetaprogrammingMacroHostProcessCacheArtifactPath(out_dir, emit_prefix),
            artifact_json);
}

void WriteInteropBridgeHeaderArtifact(const std::filesystem::path &out_dir,
                                      const std::string &emit_prefix,
                                      const std::string &artifact_text) {
  WriteText(BuildInteropBridgeHeaderArtifactPath(out_dir, emit_prefix),
            artifact_text);
}

void WriteInteropBridgeModuleArtifact(const std::filesystem::path &out_dir,
                                      const std::string &emit_prefix,
                                      const std::string &artifact_text) {
  WriteText(BuildInteropBridgeModuleArtifactPath(out_dir, emit_prefix),
            artifact_text);
}

void WriteInteropBridgeArtifact(const std::filesystem::path &out_dir,
                                const std::string &emit_prefix,
                                const std::string &artifact_json) {
  WriteText(BuildInteropBridgeArtifactPath(out_dir, emit_prefix), artifact_json);
}
