#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section_publication.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section_publication_count_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section_publication_identity_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section_publication_state_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section_publication_symbol_rows.h"

void EmitObjc3IRRuntimeMetadataSectionPublicationNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRRuntimeMetadataSectionPublicationNode(metadata, out);
  EmitObjc3IRRuntimeMetadataSectionPublicationStateFields(metadata, out);
  EmitObjc3IRRuntimeMetadataSectionPublicationCountFields(metadata, out);
  EmitObjc3IRRuntimeMetadataSectionPublicationSymbolFields(metadata, out);
  EndObjc3IRRuntimeMetadataSectionPublicationNode(out);
}
