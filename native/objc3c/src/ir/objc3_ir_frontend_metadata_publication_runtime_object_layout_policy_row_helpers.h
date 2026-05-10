#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>

void BeginObjc3IRRuntimeLayoutPolicyMetadataRow(
    const char *metadata_node_id, const std::string &first_field,
    std::ostringstream &out);
void EmitObjc3IRRuntimeLayoutPolicyStringField(
    const std::string &field_value, std::ostringstream &out);
void EmitObjc3IRRuntimeLayoutPolicyBoolField(bool field_value,
                                             std::ostringstream &out);
void EmitObjc3IRRuntimeLayoutPolicySizeField(std::size_t field_value,
                                             std::ostringstream &out);
void EndObjc3IRRuntimeLayoutPolicyMetadataRow(std::ostringstream &out);
