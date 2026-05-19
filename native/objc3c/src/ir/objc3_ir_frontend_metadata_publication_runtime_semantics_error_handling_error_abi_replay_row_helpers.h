#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>

void BeginObjc3IRErrorAbiReplayMetadataNode(const char *metadata_node_id,
                                            const std::string &first_field,
                                            std::ostringstream &out);
void EmitObjc3IRErrorAbiReplayStringField(const std::string &field_value,
                                          std::ostringstream &out);
void EmitObjc3IRErrorAbiReplaySizeField(std::size_t field_value,
                                        std::ostringstream &out);
void EmitObjc3IRErrorAbiReplayBoolField(bool field_value,
                                        std::ostringstream &out);
void EndObjc3IRErrorAbiReplayMetadataNode(std::ostringstream &out);
