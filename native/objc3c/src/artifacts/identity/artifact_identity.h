#pragma once

#include <filesystem>
#include <string>

#include "pipeline/results/phase_result.h"

namespace objc3::artifacts::identity {

[[nodiscard]] std::string BuildObjc3TranslationUnitIdentityKey(
    const std::filesystem::path &input_path,
    const Objc3ParseLoweringReadinessSurface &parse_lowering_readiness_surface);

}  // namespace objc3::artifacts::identity
