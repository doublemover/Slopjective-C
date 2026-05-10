#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section_publication_count_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section_publication_row_helpers.h"

void EmitObjc3IRRuntimeMetadataSectionPublicationCountFields(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeBoundarySectionPublicationSizeField(
      metadata.runtime_metadata_section_publication_class_descriptor_count, out);
  EmitObjc3IRRuntimeBoundarySectionPublicationSizeField(
      metadata.runtime_metadata_section_publication_protocol_descriptor_count,
      out);
  EmitObjc3IRRuntimeBoundarySectionPublicationSizeField(
      metadata.runtime_metadata_section_publication_category_descriptor_count,
      out);
  EmitObjc3IRRuntimeBoundarySectionPublicationSizeField(
      metadata.runtime_metadata_section_publication_property_descriptor_count,
      out);
  EmitObjc3IRRuntimeBoundarySectionPublicationSizeField(
      metadata.runtime_metadata_section_publication_ivar_descriptor_count, out);
  EmitObjc3IRRuntimeBoundarySectionPublicationSizeField(
      metadata.runtime_metadata_section_publication_total_descriptor_count, out);
  EmitObjc3IRRuntimeBoundarySectionPublicationSizeField(
      metadata.runtime_metadata_section_publication_total_retained_global_count,
      out);
}
