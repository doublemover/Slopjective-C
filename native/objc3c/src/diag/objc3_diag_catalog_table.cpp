#include "diag/objc3_diag_catalog_table.h"

#include <array>
#include <cstddef>

#include "diag/objc3_diag_catalog_artifact_tooling_data.h"
#include "diag/objc3_diag_catalog_frontend_data.h"
#include "diag/objc3_diag_catalog_runtime_api_data.h"

namespace {

inline constexpr std::size_t kNativeDiagCatalogEntryCount =
    kNativeDiagFrontendCatalogEntryCount +
    kNativeDiagRuntimeApiCatalogEntryCount +
    kNativeDiagArtifactToolingCatalogEntryCount;

void AppendCatalogEntries(
    std::array<NativeDiagCodeCatalogEntry, kNativeDiagCatalogEntryCount>
        &entries,
    std::size_t &next, std::span<const NativeDiagCodeCatalogEntry> source) {
  for (const NativeDiagCodeCatalogEntry &entry : source) {
    entries[next++] = entry;
  }
}

std::array<NativeDiagCodeCatalogEntry, kNativeDiagCatalogEntryCount>
BuildNativeDiagCodeCatalogEntries() {
  std::array<NativeDiagCodeCatalogEntry, kNativeDiagCatalogEntryCount> entries{};
  std::size_t next = 0;
  AppendCatalogEntries(entries, next, NativeDiagFrontendCatalogData());
  AppendCatalogEntries(entries, next, NativeDiagRuntimeApiCatalogData());
  AppendCatalogEntries(entries, next, NativeDiagArtifactToolingCatalogData());
  return entries;
}

const std::array<NativeDiagCodeCatalogEntry, kNativeDiagCatalogEntryCount>
    kNativeDiagCodeCatalogEntries = BuildNativeDiagCodeCatalogEntries();

}  // namespace

std::span<const NativeDiagCodeCatalogEntry> NativeDiagCodeCatalogEntries() {
  return std::span<const NativeDiagCodeCatalogEntry>(
      kNativeDiagCodeCatalogEntries.data(), kNativeDiagCodeCatalogEntries.size());
}
