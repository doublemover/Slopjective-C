#include "io/objc3_manifest_artifacts.h"

#include <string>

#include "ast/objc3_ast.h"
#include "io/objc3_file_io.h"

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
