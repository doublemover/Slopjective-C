#pragma once

#include <cstddef>

#include "diag/objc3_diag_catalog.h"

struct NativeDiagCatalogSummary {
  std::size_t entry_count = 0;
  std::size_t family_count = 0;
  bool families_unique = false;
  bool ranges_valid = false;
  bool subsystem_mappings_complete = false;
  bool owner_mappings_complete = false;
  bool hard_cutover_flags_consistent = false;
};

NativeDiagCatalogSummary BuildNativeDiagCatalogSummary();
const NativeDiagCodeCatalogEntry *FindNativeDiagCodeCatalogEntryByFamily(
    char family);
bool NativeDiagCatalogHasFamily(char family);
