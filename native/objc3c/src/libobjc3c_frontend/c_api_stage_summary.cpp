#include "libobjc3c_frontend/c_api.h"

namespace {

bool IsDefinedFrontendCApiStage(objc3c_frontend_c_stage_id_t stage) {
  switch (stage) {
    case OBJC3C_FRONTEND_STAGE_LEX:
    case OBJC3C_FRONTEND_STAGE_PARSE:
    case OBJC3C_FRONTEND_STAGE_SEMA:
    case OBJC3C_FRONTEND_STAGE_LOWER:
    case OBJC3C_FRONTEND_STAGE_EMIT:
      return true;
  }
  return false;
}

}  // namespace

extern "C" OBJC3C_FRONTEND_API uint8_t
objc3c_frontend_c_stage_summary_is_well_formed(
    const objc3c_frontend_c_stage_summary_t *summary,
    objc3c_frontend_c_stage_id_t expected_stage) {
  if (!IsDefinedFrontendCApiStage(expected_stage) || summary == nullptr ||
      summary->stage != expected_stage) {
    return 0u;
  }
  if (summary->attempted > 1u || summary->skipped > 1u) {
    return 0u;
  }
  if (summary->attempted != 0u && summary->skipped != 0u) {
    return 0u;
  }

  const uint32_t severity_total =
      summary->diagnostics_notes + summary->diagnostics_warnings +
      summary->diagnostics_errors + summary->diagnostics_fatals;
  return severity_total == summary->diagnostics_total ? 1u : 0u;
}

extern "C" OBJC3C_FRONTEND_API uint8_t
objc3c_frontend_c_stage_summary_has_diagnostics(
    const objc3c_frontend_c_stage_summary_t *summary) {
  return summary != nullptr && summary->diagnostics_total != 0u ? 1u : 0u;
}

extern "C" OBJC3C_FRONTEND_API uint8_t
objc3c_frontend_c_stage_summary_has_errors(
    const objc3c_frontend_c_stage_summary_t *summary) {
  return summary != nullptr &&
                 (summary->diagnostics_errors != 0u ||
                  summary->diagnostics_fatals != 0u)
             ? 1u
             : 0u;
}
