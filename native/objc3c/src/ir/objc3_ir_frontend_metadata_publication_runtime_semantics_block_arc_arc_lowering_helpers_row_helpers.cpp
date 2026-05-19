#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_lowering_helpers_row_helpers.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"

void BeginObjc3IRArcLoweringHelperMetadataNode(const char *metadata_node_id,
                                               const char *first_field,
                                               std::ostringstream &out) {
  out << metadata_node_id << " = !{!\"" << EscapeCStringLiteral(first_field)
      << "\"";
}

void EmitObjc3IRArcLoweringHelperStringField(const char *field_value,
                                             std::ostringstream &out) {
  out << ", !\"" << EscapeCStringLiteral(field_value) << "\"";
}

void EndObjc3IRArcLoweringHelperMetadataNode(std::ostringstream &out) {
  out << "}\n";
}
