#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_base_link_rows.h"

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_base_row_helpers.h"

void EmitObjc3IRRuntimeSupportLibraryBaseLinkFields(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeSupportLibraryBaseStringField(
      metadata.runtime_support_library_register_image_symbol, out);
  EmitObjc3IRRuntimeSupportLibraryBaseStringField(
      metadata.runtime_support_library_lookup_selector_symbol, out);
  EmitObjc3IRRuntimeSupportLibraryBaseStringField(
      metadata.runtime_support_library_dispatch_i32_symbol, out);
  EmitObjc3IRRuntimeSupportLibraryBaseStringField(
      metadata.runtime_support_library_reset_for_testing_symbol, out);
  EmitObjc3IRRuntimeSupportLibraryBaseStringField(
      metadata.runtime_support_library_driver_link_mode, out);
  EmitObjc3IRRuntimeSupportLibraryBaseStringField(
      metadata.runtime_support_library_compiler_ownership_boundary, out);
  EmitObjc3IRRuntimeSupportLibraryBaseStringField(
      metadata.runtime_support_library_runtime_ownership_boundary, out);
}
