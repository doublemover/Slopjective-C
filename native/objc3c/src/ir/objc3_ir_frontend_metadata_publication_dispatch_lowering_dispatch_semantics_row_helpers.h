#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>

void BeginObjc3IRDispatchSemanticsCounterRow(
    const char *metadata_node_id, std::size_t first_counter,
    std::ostringstream &out);
void EmitObjc3IRDispatchSemanticsSizeField(std::size_t field_value,
                                           std::ostringstream &out);
void EmitObjc3IRDispatchSemanticsBoolField(bool field_value,
                                           std::ostringstream &out);
void EmitObjc3IRDispatchSemanticsStringField(const std::string &field_value,
                                             std::ostringstream &out);
void EndObjc3IRDispatchSemanticsCounterRow(std::ostringstream &out);
