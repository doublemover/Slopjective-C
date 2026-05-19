#pragma once

#include <cstddef>

struct Objc3IRFrontendMetadata;

struct Objc3IRRuntimeProtocolCategoryCategoryCounts {
  std::size_t bundle_count = 0;
  std::size_t adopted_reference_total = 0;
  std::size_t attachment_reference_total = 0;
};

Objc3IRRuntimeProtocolCategoryCategoryCounts
CollectObjc3IRRuntimeProtocolCategoryCategoryCounts(
    const Objc3IRFrontendMetadata &metadata);
