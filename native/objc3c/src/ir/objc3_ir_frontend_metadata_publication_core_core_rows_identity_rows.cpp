#include "ir/objc3_ir_frontend_metadata_publication_core_core_rows_identity_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_core_core_rows_row_helpers.h"

void EmitObjc3IRFrontendInterfaceImplementationCoreCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRFrontendCoreCounterRow("!1", metadata.declared_interfaces, out);
  EmitObjc3IRFrontendCoreSizeField(metadata.declared_implementations, out);
  EmitObjc3IRFrontendCoreSizeField(metadata.resolved_interface_symbols, out);
  EmitObjc3IRFrontendCoreSizeField(metadata.resolved_implementation_symbols,
                                   out);
  EmitObjc3IRFrontendCoreSizeField(metadata.interface_method_symbols, out);
  EmitObjc3IRFrontendCoreSizeField(metadata.implementation_method_symbols, out);
  EmitObjc3IRFrontendCoreSizeField(metadata.linked_implementation_symbols, out);
  EmitObjc3IRFrontendCoreBoolField(
      metadata.deterministic_interface_implementation_handoff, out);
  EndObjc3IRFrontendCoreCounterRow(false, out);
}

void EmitObjc3IRFrontendProtocolCategoryCoreCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRFrontendCoreCounterRow("!2", metadata.declared_protocols, out);
  EmitObjc3IRFrontendCoreSizeField(metadata.declared_categories, out);
  EmitObjc3IRFrontendCoreSizeField(metadata.resolved_protocol_symbols, out);
  EmitObjc3IRFrontendCoreSizeField(metadata.resolved_category_symbols, out);
  EmitObjc3IRFrontendCoreSizeField(metadata.protocol_method_symbols, out);
  EmitObjc3IRFrontendCoreSizeField(metadata.category_method_symbols, out);
  EmitObjc3IRFrontendCoreSizeField(metadata.linked_category_symbols, out);
  EmitObjc3IRFrontendCoreBoolField(
      metadata.deterministic_protocol_category_handoff, out);
  EndObjc3IRFrontendCoreCounterRow(false, out);
}
