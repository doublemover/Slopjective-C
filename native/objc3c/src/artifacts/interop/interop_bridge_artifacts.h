#pragma once

#include <string>

#include "ast/objc3_ast.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::interop {

[[nodiscard]] std::string BuildInteropBridgeHeaderArtifactText(
    const Objc3Program &program,
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3InteropHeaderModuleBridgeGenerationSummary &bridge_summary);

[[nodiscard]] std::string BuildInteropBridgeModuleArtifactText(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3InteropHeaderModuleBridgeGenerationSummary &bridge_summary);

[[nodiscard]] std::string BuildInteropBridgeArtifactJson(
    const Objc3Program &program,
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3InteropHeaderModuleBridgeGenerationSummary &bridge_summary);

}  // namespace objc3::artifacts::interop
