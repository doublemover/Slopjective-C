#include "pipeline/frontend_runtime_export_enforcement_helpers_owners.h"

namespace objc3c::pipeline::orchestration {

void PublishRuntimeExportFailureLocationAndReason(
    Objc3RuntimeExportEnforcementSummary &summary,
    const Objc3RuntimeExportViolationAccumulator &duplicate_violations,
    const Objc3RuntimeExportViolationAccumulator &incomplete_violations,
    const Objc3RuntimeExportViolationAccumulator &illegal_redeclaration_violations) {
  if (duplicate_violations.has_location) {
    summary.first_failure_line = duplicate_violations.first_line;
    summary.first_failure_column = duplicate_violations.first_column;
  } else if (incomplete_violations.has_location) {
    summary.first_failure_line = incomplete_violations.first_line;
    summary.first_failure_column = incomplete_violations.first_column;
  } else if (illegal_redeclaration_violations.has_location) {
    summary.first_failure_line = illegal_redeclaration_violations.first_line;
    summary.first_failure_column =
        illegal_redeclaration_violations.first_column;
  }

  if (summary.duplicate_runtime_identity_sites > 0u) {
    summary.failure_reason =
        "duplicate runtime metadata identities are not exportable";
  } else if (summary.incomplete_declaration_sites > 0u) {
    summary.failure_reason =
        "incomplete runtime metadata declarations are not exportable";
  } else if (summary.illegal_redeclaration_mix_sites > 0u) {
    summary.failure_reason =
        "illegal runtime metadata redeclaration mixes are not exportable";
  } else if (summary.metadata_shape_drift_sites > 0u) {
    summary.failure_reason =
        "runtime metadata export shape drift detected before lowering";
  }
}

}  // namespace objc3c::pipeline::orchestration
