#pragma once

#include <string>
#include <vector>

namespace objc3::artifacts {

std::vector<std::string> BuildRunnableFeatureClaimIds();
std::vector<std::string> BuildSourceOnlyFeatureClaimIds();
std::vector<std::string> BuildUnsupportedFeatureClaimIds();
std::vector<std::string> BuildSupportedSelectionSurfaceIds();
std::vector<std::string> BuildUnsupportedSelectionSurfaceIds();
std::vector<std::string> BuildSuppressedMacroClaimIds();

}  // namespace objc3::artifacts
