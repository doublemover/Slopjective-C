#include "diag/objc3_diag_catalog_contract.h"

#include "diag/objc3_diag_catalog_table.h"

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
