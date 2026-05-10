#pragma once

#include <iosfwd>
#include <string>

void BeginObjc3IRRuntimeSupportLibraryBaseMetadataRow(
    const char *metadata_node_id, const std::string &first_field,
    std::ostringstream &out);
void EmitObjc3IRRuntimeSupportLibraryBaseStringField(
    const std::string &field_value, std::ostringstream &out);
void EmitObjc3IRRuntimeSupportLibraryBaseBoolField(bool field_value,
                                                   std::ostringstream &out);
void EndObjc3IRRuntimeSupportLibraryBaseMetadataRow(std::ostringstream &out);
