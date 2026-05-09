#include "diag/objc3_diag_catalog_runtime_api_data.h"

#include <array>

namespace {

constexpr std::array<NativeDiagCodeCatalogEntry,
                     kNativeDiagRuntimeApiCatalogEntryCount>
    kNativeDiagRuntimeApiCatalog = {{
        {'R', Objc3DiagnosticSubsystem::kRuntime,
         Objc3DiagnosticCategory::kRuntime,
         "runtime dispatch, C API, and metadata diagnostics", 1, 399,
         kObjc3RuntimeDiagnosticCatalogOwner},
        {'E', Objc3DiagnosticSubsystem::kFrontendApi,
         Objc3DiagnosticCategory::kFrontendApi,
         "public frontend C API and embedding diagnostics", 1, 99,
         kObjc3FrontendApiDiagnosticCatalogOwner},
    }};

}  // namespace

std::span<const NativeDiagCodeCatalogEntry> NativeDiagRuntimeApiCatalogData() {
  return std::span<const NativeDiagCodeCatalogEntry>(
      kNativeDiagRuntimeApiCatalog.data(), kNativeDiagRuntimeApiCatalog.size());
}
