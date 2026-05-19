#pragma once

#include <iosfwd>
#include <string>

void BeginObjc3IRRuntimeSupportLibraryCoreFeatureMetadataRow(
    const char *metadata_node_id, const std::string &contract_id,
    std::ostringstream &out);
void EmitObjc3IRRuntimeSupportLibraryCoreFeatureBoolField(
    bool field_value, std::ostringstream &out);
void EmitObjc3IRRuntimeSupportLibraryCoreFeatureStringField(
    const std::string &field_value, std::ostringstream &out);
void EndObjc3IRRuntimeSupportLibraryCoreFeatureMetadataRow(
    std::ostringstream &out);
