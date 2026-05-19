#pragma once

#include <cstddef>

#include "pipeline/frontend_runtime_export_enforcement_helpers.h"

namespace objc3c::pipeline::orchestration {

struct Objc3RuntimeExportViolationAccumulator {
  std::size_t count = 0;
  unsigned first_line = 1;
  unsigned first_column = 1;
  bool has_location = false;

  void Add(std::size_t increment, unsigned line, unsigned column);
  void Merge(const Objc3RuntimeExportViolationAccumulator &other);
};

Objc3RuntimeExportViolationAccumulator
CountDuplicateRuntimeExportIdentitySites(
    const Objc3RuntimeMetadataSourceRecordSet &records);

Objc3RuntimeExportViolationAccumulator
CountIncompleteRuntimeExportDeclarationSites(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality);

Objc3RuntimeExportViolationAccumulator
CountIllegalRuntimeExportRedeclarationSites(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality);

std::size_t CountRuntimeExportMetadataShapeDriftSites(
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality);

void PublishRuntimeExportFailureLocationAndReason(
    Objc3RuntimeExportEnforcementSummary &summary,
    const Objc3RuntimeExportViolationAccumulator &duplicate_violations,
    const Objc3RuntimeExportViolationAccumulator &incomplete_violations,
    const Objc3RuntimeExportViolationAccumulator &illegal_redeclaration_violations);

}  // namespace objc3c::pipeline::orchestration
