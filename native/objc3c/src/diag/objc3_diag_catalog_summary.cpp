#include "diag/objc3_diag_catalog_contract.h"

#include "diag/objc3_diag_catalog_table.h"

#include <set>

NativeDiagCatalogSummary BuildNativeDiagCatalogSummary() {
  NativeDiagCatalogSummary summary;
  std::set<char> families;
  summary.ranges_valid = true;
  summary.subsystem_mappings_complete = true;
  summary.owner_mappings_complete = true;
  summary.hard_cutover_flags_consistent = true;
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
    summary.owner_mappings_complete =
        summary.owner_mappings_complete &&
        Objc3DiagnosticOwnerIsExplicit(entry.catalog_owner);
    summary.hard_cutover_flags_consistent =
        summary.hard_cutover_flags_consistent &&
        !entry.legacy_positive_allowed && !entry.fallback_allowed;
  }
  summary.family_count = families.size();
  summary.families_unique = summary.family_count == summary.entry_count;
  return summary;
}
