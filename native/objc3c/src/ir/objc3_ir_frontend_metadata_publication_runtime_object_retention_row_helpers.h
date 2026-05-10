#pragma once

#include <iosfwd>
#include <string>

void BeginObjc3IRRuntimeObjectRetentionMetadataRow(
    const char *metadata_node_id, const std::string &first_field,
    std::ostringstream &out);
void EmitObjc3IRRuntimeObjectRetentionStringField(
    const std::string &field_value, std::ostringstream &out);
void EmitObjc3IRRuntimeObjectRetentionBoolField(bool field_value,
                                                std::ostringstream &out);
void EndObjc3IRRuntimeObjectRetentionMetadataRow(std::ostringstream &out);
