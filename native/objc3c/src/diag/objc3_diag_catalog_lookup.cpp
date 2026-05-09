#include "diag/objc3_diag_catalog.h"

#include "diag/objc3_diag_catalog_table.h"

const NativeDiagCodeCatalogEntry *FindNativeDiagCodeCatalogEntry(
    std::string_view candidate) {
  Objc3DiagnosticCode parsed;
  if (!TryParseNativeDiagCode(candidate, parsed)) {
    return nullptr;
  }
  for (const auto &entry : NativeDiagCodeCatalogEntries()) {
    if (entry.family == candidate[2] && entry.subsystem == parsed.subsystem &&
        parsed.ordinal >= entry.min_ordinal &&
        parsed.ordinal <= entry.max_ordinal) {
      return &entry;
    }
  }
  return nullptr;
}
