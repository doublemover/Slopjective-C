#pragma once

#include <cstddef>
#include <iosfwd>

void EmitObjc3IRMemoryManagementRuntimeApiMetadataNode(
    std::size_t synthesized_property_accessor_count, std::ostringstream &out);
