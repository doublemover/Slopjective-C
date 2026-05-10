#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "pipeline/frontend_runtime_export_enforcement_helpers.h"

namespace objc3c::pipeline::orchestration {

struct Objc3RuntimeExportDiagnosticPairPresence {
  std::size_t interface_records = 0;
  std::size_t implementation_records = 0;
  unsigned line = 1;
  unsigned column = 1;
  bool has_location = false;
};

struct Objc3RuntimeExportDiagnosticDuplicateSite {
  std::size_t count = 0;
  unsigned line = 1;
  unsigned column = 1;
  bool has_location = false;
};

inline void CaptureRuntimeExportDiagnosticLocation(
    Objc3RuntimeExportDiagnosticPairPresence &presence, unsigned line,
    unsigned column) {
  if (!presence.has_location) {
    presence.line = line;
    presence.column = column;
    presence.has_location = true;
  }
}

inline void CaptureRuntimeExportDiagnosticLocation(
    Objc3RuntimeExportDiagnosticDuplicateSite &presence, unsigned line,
    unsigned column) {
  if (!presence.has_location) {
    presence.line = line;
    presence.column = column;
    presence.has_location = true;
  }
}

inline const char *Objc3RuntimeExportRecordNoun(std::size_t count) {
  return count == 1u ? "record" : "records";
}

inline void AppendRuntimeExportBlockingDiagnostic(
    std::vector<Objc3RuntimeExportBlockingDiagnostic> &diagnostics,
    unsigned line, unsigned column, const std::string &code,
    const std::string &message) {
  diagnostics.push_back({line, column, code, message});
}

void AppendDuplicateRuntimeExportBlockingDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    std::vector<Objc3RuntimeExportBlockingDiagnostic> &diagnostics);

void AppendIncompleteRuntimeExportBlockingDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportEnforcementSummary &summary,
    std::vector<Objc3RuntimeExportBlockingDiagnostic> &diagnostics);

}  // namespace objc3c::pipeline::orchestration
