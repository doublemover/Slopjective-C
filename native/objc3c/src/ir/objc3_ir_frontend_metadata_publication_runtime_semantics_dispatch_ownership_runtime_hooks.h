#pragma once

#include <cstddef>
#include <iosfwd>

void EmitObjc3IROwnershipRuntimeHookEmissionMetadataNode(
    std::size_t synthesized_property_accessor_count, std::ostringstream &out);
