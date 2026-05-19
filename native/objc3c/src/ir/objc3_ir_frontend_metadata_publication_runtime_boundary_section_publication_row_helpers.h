#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>

void BeginObjc3IRRuntimeBoundarySectionPublicationRow(
    const char *metadata_node_id, const std::string &first_field,
    std::ostringstream &out);
void EmitObjc3IRRuntimeBoundarySectionPublicationStringField(
    const std::string &field_value, std::ostringstream &out);
void EmitObjc3IRRuntimeBoundarySectionPublicationBoolField(
    bool field_value, std::ostringstream &out);
void EmitObjc3IRRuntimeBoundarySectionPublicationSizeField(
    std::size_t field_value, std::ostringstream &out);
void EndObjc3IRRuntimeBoundarySectionPublicationRow(std::ostringstream &out);
