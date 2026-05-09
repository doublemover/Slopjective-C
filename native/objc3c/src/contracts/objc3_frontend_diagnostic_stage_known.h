#pragma once

#include "contracts/objc3_frontend_diagnostic_stage_name.h"

inline bool Objc3FrontendDiagnosticStageIsKnown(
    Objc3FrontendDiagnosticStage stage) {
  return !Objc3FrontendDiagnosticStageName(stage).empty();
}
