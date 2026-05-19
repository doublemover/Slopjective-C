#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>

void BeginObjc3IRRuntimeSupportDebugProjectionMetadataRow(
    const char *metadata_node_id, const std::string &first_field,
    std::ostringstream &out);
void EmitObjc3IRRuntimeSupportDebugProjectionStringField(
    const std::string &field_value, std::ostringstream &out);
void EmitObjc3IRRuntimeSupportDebugProjectionBoolField(
    bool field_value, std::ostringstream &out);
void EmitObjc3IRRuntimeSupportDebugProjectionSizeField(
    std::size_t field_value, std::ostringstream &out);
void EndObjc3IRRuntimeSupportDebugProjectionMetadataRow(
    std::ostringstream &out);
