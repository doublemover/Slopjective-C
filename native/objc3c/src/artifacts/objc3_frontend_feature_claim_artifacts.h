#pragma once

#include <string>
#include <vector>

struct Objc3FrontendOptions;
struct Objc3FrontendPipelineResult;

namespace objc3::artifacts {

std::vector<std::string> BuildRunnableFeatureClaimIds();
std::vector<std::string> BuildSourceOnlyFeatureClaimIds();
std::vector<std::string> BuildUnsupportedFeatureClaimIds();
std::vector<std::string> BuildSupportedSelectionSurfaceIds();
std::vector<std::string> BuildUnsupportedSelectionSurfaceIds();
std::vector<std::string> BuildSuppressedMacroClaimIds();

std::string BuildRunnableFeatureClaimInventoryReplayKey(
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result);

std::string BuildRunnableFeatureClaimInventoryJson(
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result);

}  // namespace objc3::artifacts
