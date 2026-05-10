#pragma once

#include <cstddef>
#include <iosfwd>

void EmitObjc3IRRuntimeObjectPoolMetadataNode(
    std::size_t selector_pool_global_count,
    std::size_t runtime_string_pool_global_count, std::ostringstream &out);
