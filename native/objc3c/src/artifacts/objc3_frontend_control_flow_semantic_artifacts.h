#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string BuildControlFlowControlFlowSemanticModelSummaryJson(
    const Objc3ControlFlowControlFlowSemanticModelSummary &summary);

}  // namespace objc3::artifacts::frontend
