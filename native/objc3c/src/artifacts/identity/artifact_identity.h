#pragma once

#include <filesystem>
#include <string>

namespace objc3::artifacts::identity {

struct Objc3TranslationUnitIdentityEvidence {
  std::filesystem::path input_path;
  std::string parse_artifact_replay_key;
  std::string lowering_boundary_replay_key;
};

[[nodiscard]] std::string BuildObjc3TranslationUnitIdentityKey(
    const Objc3TranslationUnitIdentityEvidence &evidence);

}  // namespace objc3::artifacts::identity
