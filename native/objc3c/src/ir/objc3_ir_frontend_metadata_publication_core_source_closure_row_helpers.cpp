#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_row_helpers.h"

#include <sstream>

void BeginObjc3IRFrontendSourceClosureAnchorComment(
    const char *anchor_name, const std::string &contract_id,
    std::ostringstream &out) {
  out << "; " << anchor_name << " = contract=" << contract_id;
}

void EmitObjc3IRFrontendSourceClosureStringField(
    const char *field_name, const std::string &field_value,
    std::ostringstream &out) {
  out << ";" << field_name << "=" << field_value;
}

void EmitObjc3IRFrontendSourceClosureSizeField(
    const char *field_name, std::size_t field_value, std::ostringstream &out) {
  out << ";" << field_name << "=" << field_value;
}

void EndObjc3IRFrontendSourceClosureAnchorComment(std::ostringstream &out) {
  out << "\n";
}
