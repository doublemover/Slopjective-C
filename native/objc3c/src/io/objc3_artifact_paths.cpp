#include "io/objc3_artifact_paths.h"

std::filesystem::path BuildManifestArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + ".manifest.json");
}
