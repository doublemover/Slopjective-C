#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_base_identity_rows.h"

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_base_row_helpers.h"

void BeginObjc3IRRuntimeSupportLibraryBaseMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRRuntimeSupportLibraryBaseMetadataRow(
      "!51", metadata.runtime_support_library_contract_id, out);
  EmitObjc3IRRuntimeSupportLibraryBaseStringField(
      metadata.runtime_support_library_metadata_publication_contract_id, out);
}

void EndObjc3IRRuntimeSupportLibraryBaseMetadataNode(std::ostringstream &out) {
  EndObjc3IRRuntimeSupportLibraryBaseMetadataRow(out);
}
