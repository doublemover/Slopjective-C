#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_memory_management_row_helpers.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"

void BeginObjc3IRMemoryManagementRuntimeMetadataNode(
    const char *metadata_node_id, const char *first_field,
    std::ostringstream &out) {
  out << metadata_node_id << " = !{!\"" << EscapeCStringLiteral(first_field)
      << "\"";
}

void EmitObjc3IRMemoryManagementRuntimeStringField(const char *field_value,
                                                   std::ostringstream &out) {
  out << ", !\"" << EscapeCStringLiteral(field_value) << "\"";
}

void EmitObjc3IRMemoryManagementRuntimeSizeField(std::size_t field_value,
                                                 std::ostringstream &out) {
  out << ", i64 " << static_cast<unsigned long long>(field_value);
}

void EndObjc3IRMemoryManagementRuntimeMetadataNode(std::ostringstream &out) {
  out << "}\n";
}
