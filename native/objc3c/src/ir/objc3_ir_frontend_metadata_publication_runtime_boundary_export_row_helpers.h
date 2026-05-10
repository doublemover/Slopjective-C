#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>

void BeginObjc3IRRuntimeBoundaryExportMetadataRow(
    const char *metadata_node_id, const std::string &contract_id,
    std::ostringstream &out);
void EmitObjc3IRRuntimeBoundaryExportBoolField(bool field_value,
                                               std::ostringstream &out);
void EmitObjc3IRRuntimeBoundaryExportSizeField(std::size_t field_value,
                                               std::ostringstream &out);
void EndObjc3IRRuntimeBoundaryExportMetadataRow(std::ostringstream &out);
