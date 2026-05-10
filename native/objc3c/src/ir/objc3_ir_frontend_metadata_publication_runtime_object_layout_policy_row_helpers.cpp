#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy_row_helpers.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"

void BeginObjc3IRRuntimeLayoutPolicyMetadataRow(
    const char *metadata_node_id, const std::string &first_field,
    std::ostringstream &out) {
  out << metadata_node_id << " = !{!\"" << EscapeCStringLiteral(first_field)
      << "\"";
}

void EmitObjc3IRRuntimeLayoutPolicyStringField(
    const std::string &field_value, std::ostringstream &out) {
  out << ", !\"" << EscapeCStringLiteral(field_value) << "\"";
}

void EmitObjc3IRRuntimeLayoutPolicyBoolField(bool field_value,
                                             std::ostringstream &out) {
  out << ", i1 " << (field_value ? 1 : 0);
}

void EmitObjc3IRRuntimeLayoutPolicySizeField(std::size_t field_value,
                                             std::ostringstream &out) {
  out << ", i64 " << static_cast<unsigned long long>(field_value);
}

void EndObjc3IRRuntimeLayoutPolicyMetadataRow(std::ostringstream &out) {
  out << "}\n";
}
