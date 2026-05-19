#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section_publication_state_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section_publication_row_helpers.h"

void EmitObjc3IRRuntimeMetadataSectionPublicationStateFields(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeBoundarySectionPublicationBoolField(
      metadata.runtime_metadata_section_publication_emitted, out);
  EmitObjc3IRRuntimeBoundarySectionPublicationBoolField(
      metadata.runtime_metadata_section_publication_fail_closed, out);
  EmitObjc3IRRuntimeBoundarySectionPublicationBoolField(
      metadata.runtime_metadata_section_publication_uses_llvm_used, out);
  EmitObjc3IRRuntimeBoundarySectionPublicationBoolField(
      metadata.runtime_metadata_section_publication_image_info_emitted, out);
}
