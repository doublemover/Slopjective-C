#pragma once

#include <string>
#include <vector>

#include "runtime/metadata/class_metadata.h"

namespace objc3c::pipeline::orchestration {

struct Objc3RuntimeExportBlockingDiagnostic {
  unsigned line = 1;
  unsigned column = 1;
  std::string code;
  std::string message;
};

std::vector<Objc3RuntimeExportBlockingDiagnostic>
BuildRuntimeExportBlockingDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportEnforcementSummary &summary);

bool HasRuntimeMetadataSourceRecords(
    const Objc3RuntimeMetadataSourceRecordSet &records);

Objc3RuntimeExportEnforcementSummary BuildRuntimeExportEnforcementSummary(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality);

}  // namespace objc3c::pipeline::orchestration
