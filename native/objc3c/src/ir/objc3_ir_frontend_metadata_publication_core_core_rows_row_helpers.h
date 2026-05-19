#pragma once

#include <cstddef>
#include <iosfwd>

void BeginObjc3IRFrontendCoreCounterRow(const char *metadata_node_id,
                                        std::size_t first_counter,
                                        std::ostringstream &out);
void EmitObjc3IRFrontendCoreSizeField(std::size_t field_value,
                                      std::ostringstream &out);
void EmitObjc3IRFrontendCoreBoolField(bool field_value,
                                      std::ostringstream &out);
void EndObjc3IRFrontendCoreCounterRow(bool trailing_blank_line,
                                      std::ostringstream &out);
