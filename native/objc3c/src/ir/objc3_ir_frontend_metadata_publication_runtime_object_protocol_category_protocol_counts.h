#pragma once

#include <cstddef>

struct Objc3IRFrontendMetadata;

struct Objc3IRRuntimeProtocolCategoryProtocolCounts {
  std::size_t bundle_count = 0;
  std::size_t inherited_reference_total = 0;
};

Objc3IRRuntimeProtocolCategoryProtocolCounts
CollectObjc3IRRuntimeProtocolCategoryProtocolCounts(
    const Objc3IRFrontendMetadata &metadata);
