#pragma once

#include <cstddef>
#include <span>

#include "diag/objc3_diag_catalog.h"

inline constexpr std::size_t kNativeDiagArtifactToolingCatalogEntryCount = 2;

std::span<const NativeDiagCodeCatalogEntry>
NativeDiagArtifactToolingCatalogData();
