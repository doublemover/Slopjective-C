#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_dispatch_semantics_row_helpers.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"

void BeginObjc3IRDispatchSemanticsCounterRow(
    const char *metadata_node_id, std::size_t first_counter,
    std::ostringstream &out) {
  out << metadata_node_id << " = !{i64 "
      << static_cast<unsigned long long>(first_counter);
}

void EmitObjc3IRDispatchSemanticsSizeField(std::size_t field_value,
                                           std::ostringstream &out) {
  out << ", i64 " << static_cast<unsigned long long>(field_value);
}

void EmitObjc3IRDispatchSemanticsBoolField(bool field_value,
                                           std::ostringstream &out) {
  out << ", i1 " << (field_value ? 1 : 0);
}

void EmitObjc3IRDispatchSemanticsStringField(const std::string &field_value,
                                             std::ostringstream &out) {
  out << ", !\"" << EscapeCStringLiteral(field_value) << "\"";
}

void EndObjc3IRDispatchSemanticsCounterRow(std::ostringstream &out) {
  out << "}\n\n";
}
