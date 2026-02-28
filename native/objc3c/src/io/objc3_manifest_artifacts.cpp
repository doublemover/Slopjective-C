#include "io/objc3_manifest_artifacts.h"

#include <string>

#include "io/objc3_file_io.h"

void WriteManifestArtifact(const std::filesystem::path &out_dir,
                           const std::string &emit_prefix,
                           const std::string &manifest_json) {
  WriteText(out_dir / (emit_prefix + ".manifest.json"), manifest_json);
}
