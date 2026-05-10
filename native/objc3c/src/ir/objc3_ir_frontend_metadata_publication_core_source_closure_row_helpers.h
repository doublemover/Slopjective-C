#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>

void BeginObjc3IRFrontendSourceClosureAnchorComment(
    const char *anchor_name, const std::string &contract_id,
    std::ostringstream &out);
void EmitObjc3IRFrontendSourceClosureStringField(
    const char *field_name, const std::string &field_value,
    std::ostringstream &out);
void EmitObjc3IRFrontendSourceClosureSizeField(
    const char *field_name, std::size_t field_value, std::ostringstream &out);
void EndObjc3IRFrontendSourceClosureAnchorComment(std::ostringstream &out);
