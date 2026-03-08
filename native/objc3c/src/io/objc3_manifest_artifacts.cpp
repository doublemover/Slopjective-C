#include "io/objc3_manifest_artifacts.h"

#include <string>

#include "ast/objc3_ast.h"
#include "io/objc3_file_io.h"
#include "lower/objc3_lowering_contract.h"

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
