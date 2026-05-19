#include "ir/objc3_ir_frontend_metadata_publication_core_core_rows_count_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_core_core_rows_row_helpers.h"

void EmitObjc3IRFrontendSelectorNormalizationCoreCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRFrontendCoreCounterRow(
      "!3", metadata.selector_method_declaration_entries, out);
  EmitObjc3IRFrontendCoreSizeField(
      metadata.selector_normalized_method_declarations, out);
  EmitObjc3IRFrontendCoreSizeField(metadata.selector_piece_entries, out);
  EmitObjc3IRFrontendCoreSizeField(metadata.selector_piece_parameter_links,
                                   out);
  EmitObjc3IRFrontendCoreBoolField(
      metadata.deterministic_selector_normalization_handoff, out);
  EndObjc3IRFrontendCoreCounterRow(false, out);
}

void EmitObjc3IRFrontendPropertyAttributeCoreCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRFrontendCoreCounterRow("!4",
                                     metadata.property_declaration_entries,
                                     out);
  EmitObjc3IRFrontendCoreSizeField(metadata.property_attribute_entries, out);
  EmitObjc3IRFrontendCoreSizeField(metadata.property_attribute_value_entries,
                                   out);
  EmitObjc3IRFrontendCoreSizeField(metadata.property_accessor_modifier_entries,
                                   out);
  EmitObjc3IRFrontendCoreSizeField(metadata.property_getter_selector_entries,
                                   out);
  EmitObjc3IRFrontendCoreSizeField(metadata.property_setter_selector_entries,
                                   out);
  EmitObjc3IRFrontendCoreBoolField(
      metadata.deterministic_property_attribute_handoff, out);
  EndObjc3IRFrontendCoreCounterRow(true, out);
}
