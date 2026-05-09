#include "diag/objc3_diag_catalog_contract.h"

#include "diag/objc3_diag_catalog_table.h"

#include <set>

NativeDiagCatalogSummary BuildNativeDiagCatalogSummary() {
  NativeDiagCatalogSummary summary;
  std::set<char> families;
  summary.ranges_valid = true;
  summary.subsystem_mappings_complete = true;
  for (const NativeDiagCodeCatalogEntry &entry :
       NativeDiagCodeCatalogEntries()) {
    ++summary.entry_count;
    families.insert(entry.family);
    summary.ranges_valid =
        summary.ranges_valid && entry.min_ordinal <= entry.max_ordinal;
    summary.subsystem_mappings_complete =
        summary.subsystem_mappings_complete &&
        entry.subsystem != Objc3DiagnosticSubsystem::kUnknown &&
        entry.category != Objc3DiagnosticCategory::kUnknown &&
        DiagnosticSubsystemForPrefix(entry.family) == entry.subsystem;
  }
  summary.family_count = families.size();
  summary.families_unique = summary.family_count == summary.entry_count;
  return summary;
}

const NativeDiagCodeCatalogEntry *FindNativeDiagCodeCatalogEntryByFamily(
    char family) {
  for (const NativeDiagCodeCatalogEntry &entry :
       NativeDiagCodeCatalogEntries()) {
    if (entry.family == family) {
      return &entry;
    }
  }
  return nullptr;
}

bool NativeDiagCatalogHasFamily(char family) {
  return FindNativeDiagCodeCatalogEntryByFamily(family) != nullptr;
}
