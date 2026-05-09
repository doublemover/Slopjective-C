#include "libobjc3c_frontend/c_api.h"

extern "C" OBJC3C_FRONTEND_API uint8_t
objc3c_frontend_c_stage_summary_is_well_formed(
    const objc3c_frontend_c_stage_summary_t *summary,
    objc3c_frontend_c_stage_id_t expected_stage) {
  switch (expected_stage) {
    case OBJC3C_FRONTEND_STAGE_LEX:
    case OBJC3C_FRONTEND_STAGE_PARSE:
    case OBJC3C_FRONTEND_STAGE_SEMA:
    case OBJC3C_FRONTEND_STAGE_LOWER:
    case OBJC3C_FRONTEND_STAGE_EMIT:
      break;
    default:
      return 0u;
  }
  if (summary == nullptr || summary->stage != expected_stage) {
    return 0u;
  }
  if (summary->attempted > 1u || summary->skipped > 1u ||
      summary->reserved != 0u) {
    return 0u;
  }
  if (summary->attempted != 0u && summary->skipped != 0u) {
    return 0u;
  }
  const uint64_t severity_total =
      static_cast<uint64_t>(summary->diagnostics_notes) +
      static_cast<uint64_t>(summary->diagnostics_warnings) +
      static_cast<uint64_t>(summary->diagnostics_errors) +
      static_cast<uint64_t>(summary->diagnostics_fatals);
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
