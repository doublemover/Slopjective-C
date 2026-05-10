#include "artifacts/objc3_frontend_artifact_lowering_replay_manifest.h"

#include <ostream>

namespace objc3::artifacts::frontend {

void WriteLoweringReplayManifestEntries(
    std::ostream &manifest,
    std::initializer_list<LoweringReplayManifestEntry> entries) {
  for (const auto &entry : entries) {
    manifest << "  \"" << entry.manifest_key << "\":{\"replay_key\":\""
             << entry.replay_key << "\",\"" << entry.contract_field_name
             << "\":\"" << entry.contract_id
             << "\",\"deterministic_handoff\":"
             << (entry.deterministic ? "true" : "false") << "},\n";
  }
}

}  // namespace objc3::artifacts::frontend
