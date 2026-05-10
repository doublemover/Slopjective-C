#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_base_artifact_rows.h"

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_base_row_helpers.h"

void EmitObjc3IRRuntimeSupportLibraryBaseArtifactFields(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeSupportLibraryBaseStringField(
      metadata.runtime_support_library_target_name, out);
  EmitObjc3IRRuntimeSupportLibraryBaseStringField(
      metadata.runtime_support_library_public_header_path, out);
  EmitObjc3IRRuntimeSupportLibraryBaseStringField(
      metadata.runtime_support_library_source_root, out);
  EmitObjc3IRRuntimeSupportLibraryBaseStringField(
      metadata.runtime_support_library_library_kind, out);
  EmitObjc3IRRuntimeSupportLibraryBaseStringField(
      metadata.runtime_support_library_archive_basename, out);
}
