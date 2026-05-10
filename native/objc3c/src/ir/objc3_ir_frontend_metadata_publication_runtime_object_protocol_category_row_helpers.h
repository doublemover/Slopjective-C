#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>

void BeginObjc3IRRuntimeProtocolCategoryMetadataRow(
    const char *metadata_node_id, const std::string &first_field,
    std::ostringstream &out);
void EmitObjc3IRRuntimeProtocolCategoryStringField(
    const std::string &field_value, std::ostringstream &out);
void EmitObjc3IRRuntimeProtocolCategoryBoolField(bool field_value,
                                                 std::ostringstream &out);
void EmitObjc3IRRuntimeProtocolCategorySizeField(std::size_t field_value,
                                                 std::ostringstream &out);
void EndObjc3IRRuntimeProtocolCategoryMetadataRow(std::ostringstream &out);
