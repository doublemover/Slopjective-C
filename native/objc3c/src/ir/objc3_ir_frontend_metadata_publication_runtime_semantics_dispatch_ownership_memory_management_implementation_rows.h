#pragma once

#include <cstddef>
#include <iosfwd>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRMemoryManagementRuntimeImplementationMetadataNode(
    const Objc3IRFrontendMetadata &metadata,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out);
