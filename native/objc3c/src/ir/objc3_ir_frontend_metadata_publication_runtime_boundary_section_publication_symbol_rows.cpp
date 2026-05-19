#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section_publication_symbol_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section_publication_row_helpers.h"

void EmitObjc3IRRuntimeMetadataSectionPublicationSymbolFields(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeBoundarySectionPublicationStringField(
      metadata.runtime_metadata_section_publication_image_info_symbol, out);
  EmitObjc3IRRuntimeBoundarySectionPublicationStringField(
      metadata.runtime_metadata_section_publication_class_aggregate_symbol, out);
  EmitObjc3IRRuntimeBoundarySectionPublicationStringField(
      metadata.runtime_metadata_section_publication_protocol_aggregate_symbol,
      out);
  EmitObjc3IRRuntimeBoundarySectionPublicationStringField(
      metadata.runtime_metadata_section_publication_category_aggregate_symbol,
      out);
  EmitObjc3IRRuntimeBoundarySectionPublicationStringField(
      metadata.runtime_metadata_section_publication_property_aggregate_symbol,
      out);
  EmitObjc3IRRuntimeBoundarySectionPublicationStringField(
      metadata.runtime_metadata_section_publication_ivar_aggregate_symbol, out);
}
