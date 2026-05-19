#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_retention_row_helpers.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"

void BeginObjc3IRRuntimeObjectRetentionMetadataRow(
    const char *metadata_node_id, const std::string &first_field,
    std::ostringstream &out) {
  out << metadata_node_id << " = !{!\"" << EscapeCStringLiteral(first_field)
      << "\"";
}

void EmitObjc3IRRuntimeObjectRetentionStringField(
    const std::string &field_value, std::ostringstream &out) {
  out << ", !\"" << EscapeCStringLiteral(field_value) << "\"";
}

void EmitObjc3IRRuntimeObjectRetentionBoolField(bool field_value,
                                                std::ostringstream &out) {
  out << ", i1 " << (field_value ? 1 : 0);
}

void EndObjc3IRRuntimeObjectRetentionMetadataRow(std::ostringstream &out) {
  out << "}\n";
}
