#include "artifacts/identity/artifact_identity.h"

namespace objc3::artifacts::identity {

std::string BuildObjc3TranslationUnitIdentityKey(
    const std::filesystem::path &input_path,
    const Objc3ParseLoweringReadinessSurface &parse_lowering_readiness_surface) {
  return input_path.generic_string() + "|" +
         parse_lowering_readiness_surface.parse_artifact_replay_key + "|" +
         parse_lowering_readiness_surface.lowering_boundary_replay_key;
}

}  // namespace objc3::artifacts::identity
