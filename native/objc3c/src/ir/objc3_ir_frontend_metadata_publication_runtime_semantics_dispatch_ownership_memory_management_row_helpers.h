#pragma once

#include <cstddef>
#include <iosfwd>

void BeginObjc3IRMemoryManagementRuntimeMetadataNode(
    const char *metadata_node_id, const char *first_field,
    std::ostringstream &out);
void EmitObjc3IRMemoryManagementRuntimeStringField(const char *field_value,
                                                   std::ostringstream &out);
void EmitObjc3IRMemoryManagementRuntimeSizeField(std::size_t field_value,
                                                 std::ostringstream &out);
void EndObjc3IRMemoryManagementRuntimeMetadataNode(std::ostringstream &out);
