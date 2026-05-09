#include "io/objc3_artifact_writers.h"

#include "io/objc3_artifact_paths.h"
#include "io/objc3_file_io.h"

void WriteManifestArtifact(const std::filesystem::path &out_dir,
                           const std::string &emit_prefix,
                           const std::string &manifest_json) {
  WriteText(BuildManifestArtifactPath(out_dir, emit_prefix), manifest_json);
}
