#pragma once

#include <iosfwd>
#include <string>

void BeginObjc3IRConcurrencyTaskRuntimeMetadataRow(
    const char *metadata_node_id, const std::string &contract_id,
    std::ostringstream &out);
void EmitObjc3IRConcurrencyTaskRuntimeStringField(
    const std::string &field_value, std::ostringstream &out);
void EmitObjc3IRConcurrencyTaskRuntimeCommonSymbolFields(std::ostringstream &out);
void EndObjc3IRConcurrencyTaskRuntimeMetadataRow(std::ostringstream &out);
