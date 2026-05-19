#include "libobjc3c_frontend/objc3c_frontend_diagnostics.h"

#include <cctype>

namespace objc3c::frontend {

namespace {

std::string ToLowerCopy(std::string value) {
  for (char &ch : value) {
    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
  }
  return value;
}

struct StageDiagnosticCounts {
  uint32_t notes = 0;
  uint32_t warnings = 0;
  uint32_t errors = 0;
  uint32_t fatals = 0;
};

StageDiagnosticCounts CountDiagnosticsBySeverity(
    const std::vector<std::string> &diagnostics) {
  StageDiagnosticCounts counts;
  for (const std::string &diag : diagnostics) {
    const std::size_t colon = diag.find(':');
    const std::string prefix = ToLowerCopy(diag.substr(0, colon));
    if (prefix == "note") {
      ++counts.notes;
    } else if (prefix == "warning") {
      ++counts.warnings;
    } else if (prefix == "fatal") {
      ++counts.fatals;
    } else {
      ++counts.errors;
    }
  }
  return counts;
}

}  // namespace

objc3c_frontend_stage_summary_t BuildFrontendStageSummary(
    objc3c_frontend_stage_id_t stage_id,
    bool attempted,
    bool skipped,
    const std::vector<std::string> &diagnostics) {
  const StageDiagnosticCounts counts = CountDiagnosticsBySeverity(diagnostics);
  objc3c_frontend_stage_summary_t summary = {};
  summary.stage = stage_id;
  summary.attempted = attempted ? 1u : 0u;
  summary.skipped = skipped ? 1u : 0u;
  summary.diagnostics_total = static_cast<uint32_t>(diagnostics.size());
  summary.diagnostics_notes = counts.notes;
  summary.diagnostics_warnings = counts.warnings;
  summary.diagnostics_errors = counts.errors;
  summary.diagnostics_fatals = counts.fatals;
  return summary;
}

}  // namespace objc3c::frontend

extern "C" OBJC3C_FRONTEND_API uint8_t
objc3c_frontend_stage_summary_is_well_formed(
    const objc3c_frontend_stage_summary_t *summary,
    objc3c_frontend_stage_id_t expected_stage) {
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
  if (summary->attempted > 1u || summary->skipped > 1u) {
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
objc3c_frontend_stage_summary_has_diagnostics(
    const objc3c_frontend_stage_summary_t *summary) {
  return summary != nullptr && summary->diagnostics_total != 0u ? 1u : 0u;
}

extern "C" OBJC3C_FRONTEND_API uint8_t
objc3c_frontend_stage_summary_has_errors(
    const objc3c_frontend_stage_summary_t *summary) {
  return summary != nullptr &&
                 (summary->diagnostics_errors != 0u ||
                  summary->diagnostics_fatals != 0u)
             ? 1u
             : 0u;
}
