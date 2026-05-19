#include "artifacts/identity/artifact_identity.h"

namespace objc3::artifacts::identity {

std::string BuildObjc3TranslationUnitIdentityKey(
    const Objc3TranslationUnitIdentityEvidence &evidence) {
  return evidence.input_path.generic_string() + "|" +
         evidence.parse_artifact_replay_key + "|" +
         evidence.lowering_boundary_replay_key;
}

}  // namespace objc3::artifacts::identity
