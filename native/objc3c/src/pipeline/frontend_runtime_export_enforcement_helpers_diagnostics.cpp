#include "pipeline/frontend_runtime_export_enforcement_helpers.h"

#include <algorithm>
#include <string>
#include <tuple>
#include <vector>

#include "pipeline/frontend_runtime_export_enforcement_helpers_diagnostics_owners.h"

namespace objc3c::pipeline::orchestration {

std::vector<Objc3RuntimeExportBlockingDiagnostic>
BuildRuntimeExportBlockingDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportEnforcementSummary &summary) {
  std::vector<Objc3RuntimeExportBlockingDiagnostic> diagnostics;

  if (summary.duplicate_runtime_identity_sites > 0u) {
    AppendDuplicateRuntimeExportBlockingDiagnostics(records, diagnostics);
  }
  AppendIncompleteRuntimeExportBlockingDiagnostics(records, summary,
                                                  diagnostics);

  if (summary.duplicate_runtime_identity_sites > 0u && diagnostics.empty()) {
    AppendRuntimeExportBlockingDiagnostic(
        diagnostics, summary.first_failure_line, summary.first_failure_column,
        "O3S262",
        "runtime metadata export blocked: duplicate runtime metadata "
        "identities are not exportable");
  }

  std::sort(diagnostics.begin(), diagnostics.end(),
            [](const Objc3RuntimeExportBlockingDiagnostic &lhs,
               const Objc3RuntimeExportBlockingDiagnostic &rhs) {
              return std::tie(lhs.line, lhs.column, lhs.code, lhs.message) <
                     std::tie(rhs.line, rhs.column, rhs.code, rhs.message);
            });
  return diagnostics;
}

}  // namespace objc3c::pipeline::orchestration
