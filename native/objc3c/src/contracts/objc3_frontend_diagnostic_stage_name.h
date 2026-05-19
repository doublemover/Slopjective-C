#pragma once

#include <string_view>

#include "contracts/objc3_frontend_diagnostic_stage_table.h"

inline std::string_view Objc3FrontendDiagnosticStageName(
    Objc3FrontendDiagnosticStage stage) {
  for (const auto &descriptor : kObjc3FrontendDiagnosticStageTable) {
    if (descriptor.stage == stage) {
      return descriptor.name;
    }
  }
  return {};
}
