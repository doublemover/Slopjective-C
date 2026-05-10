#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section_publication_identity_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section_publication_row_helpers.h"

void BeginObjc3IRRuntimeMetadataSectionPublicationNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRRuntimeBoundarySectionPublicationRow(
      "!49", metadata.runtime_metadata_section_publication_contract_id, out);
  EmitObjc3IRRuntimeBoundarySectionPublicationStringField(
      metadata.runtime_metadata_section_publication_abi_contract_id, out);
}

void EndObjc3IRRuntimeMetadataSectionPublicationNode(std::ostringstream &out) {
  EndObjc3IRRuntimeBoundarySectionPublicationRow(out);
}
