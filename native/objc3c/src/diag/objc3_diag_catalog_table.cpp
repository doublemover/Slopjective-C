#include "diag/objc3_diag_catalog_table.h"

#include "diag/objc3_diag_catalog_data.h"

std::span<const NativeDiagCodeCatalogEntry> NativeDiagCodeCatalogEntries() {
  return NativeDiagCodeCatalogData();
}
