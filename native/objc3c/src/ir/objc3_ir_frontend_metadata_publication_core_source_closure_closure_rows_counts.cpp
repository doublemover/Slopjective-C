#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_closure_rows_counts.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_row_helpers.h"

void EmitObjc3IRFrontendProtocolCategorySourceClosureCountFields(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRFrontendSourceClosureSizeField(
      "protocol_node_count",
      metadata.executable_protocol_category_protocol_node_count, out);
  EmitObjc3IRFrontendSourceClosureSizeField(
      "category_node_count",
      metadata.executable_protocol_category_category_node_count, out);
  EmitObjc3IRFrontendSourceClosureSizeField(
      "protocol_inheritance_edge_count",
      metadata.executable_protocol_inheritance_identity_edge_count, out);
  EmitObjc3IRFrontendSourceClosureSizeField(
      "category_attachment_edge_count",
      metadata.executable_category_attachment_identity_edge_count, out);
  EmitObjc3IRFrontendSourceClosureSizeField(
      "protocol_category_conformance_edge_count",
      metadata.executable_protocol_category_conformance_identity_edge_count,
      out);
}
