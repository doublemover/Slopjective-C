#pragma once

#include <cstddef>
#include <iosfwd>

void BeginObjc3IRBlockStorageLoweringCounterRow(
    const char *metadata_node_id, std::size_t first_counter,
    std::ostringstream &out);
void EmitObjc3IRBlockStorageLoweringSizeField(std::size_t field_value,
                                              std::ostringstream &out);
void EmitObjc3IRBlockStorageLoweringBoolField(bool field_value,
                                              std::ostringstream &out);
void EndObjc3IRBlockStorageLoweringCounterRow(std::ostringstream &out);
