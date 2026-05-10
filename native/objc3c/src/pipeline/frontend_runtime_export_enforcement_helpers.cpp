#include "pipeline/frontend_runtime_export_enforcement_helpers.h"

#include "pipeline/frontend_runtime_export_enforcement_helpers_owners.h"

namespace objc3c::pipeline::orchestration {

void Objc3RuntimeExportViolationAccumulator::Add(std::size_t increment,
                                                 unsigned line,
                                                 unsigned column) {
  if (increment == 0) {
    return;
  }
  if (!has_location) {
    first_line = line;
    first_column = column;
    has_location = true;
  }
  count += increment;
}

void Objc3RuntimeExportViolationAccumulator::Merge(
    const Objc3RuntimeExportViolationAccumulator &other) {
  if (other.count == 0) {
    return;
  }
  Add(other.count, other.first_line, other.first_column);
}

bool HasRuntimeMetadataSourceRecords(
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  return !records.classes_lexicographic.empty() ||
         !records.protocols_lexicographic.empty() ||
         !records.categories_lexicographic.empty() ||
         !records.properties_lexicographic.empty() ||
         !records.methods_lexicographic.empty() ||
         !records.ivars_lexicographic.empty();
}

Objc3RuntimeExportEnforcementSummary BuildRuntimeExportEnforcementSummary(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality) {
  Objc3RuntimeExportEnforcementSummary summary;
  summary.metadata_completeness_enforced = true;
  summary.duplicate_runtime_identity_suppression_enforced = true;
  summary.illegal_redeclaration_mix_blocking_enforced = true;
  summary.metadata_shape_drift_blocking_enforced = true;
  summary.fail_closed = true;

  const Objc3RuntimeExportViolationAccumulator duplicate_violations =
      CountDuplicateRuntimeExportIdentitySites(records);
  summary.duplicate_runtime_identity_sites = duplicate_violations.count;

  const Objc3RuntimeExportViolationAccumulator incomplete_violations =
      CountIncompleteRuntimeExportDeclarationSites(records,
                                                  runtime_export_legality);
  summary.incomplete_declaration_sites = incomplete_violations.count;

  const Objc3RuntimeExportViolationAccumulator illegal_redeclaration_violations =
      CountIllegalRuntimeExportRedeclarationSites(records,
                                                 runtime_export_legality);
  summary.illegal_redeclaration_mix_sites =
      illegal_redeclaration_violations.count;

  summary.metadata_shape_drift_sites =
      CountRuntimeExportMetadataShapeDriftSites(runtime_export_legality);

  summary.ready_for_runtime_export =
      summary.duplicate_runtime_identity_sites == 0u &&
      summary.incomplete_declaration_sites == 0u &&
      summary.illegal_redeclaration_mix_sites == 0u &&
      summary.metadata_shape_drift_sites == 0u;

  PublishRuntimeExportFailureLocationAndReason(
      summary, duplicate_violations, incomplete_violations,
      illegal_redeclaration_violations);

  return summary;
}

}  // namespace objc3c::pipeline::orchestration
