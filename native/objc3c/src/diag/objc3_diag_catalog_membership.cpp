#include "diag/objc3_diag_catalog.h"

bool NativeDiagCodeIsWithinCatalog(std::string_view candidate) {
  return FindNativeDiagCodeCatalogEntry(candidate) != nullptr;
}
