#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_base_state_rows.h"

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_base_row_helpers.h"

void EmitObjc3IRRuntimeSupportLibraryBaseStateFields(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeSupportLibraryBaseBoolField(
      metadata.runtime_support_library_boundary_frozen, out);
  EmitObjc3IRRuntimeSupportLibraryBaseBoolField(
      metadata.runtime_support_library_fail_closed, out);
  EmitObjc3IRRuntimeSupportLibraryBaseBoolField(
      metadata.runtime_support_library_target_name_frozen, out);
  EmitObjc3IRRuntimeSupportLibraryBaseBoolField(
      metadata.runtime_support_library_exported_entrypoints_frozen, out);
  EmitObjc3IRRuntimeSupportLibraryBaseBoolField(
      metadata.runtime_support_library_ownership_boundaries_frozen, out);
  EmitObjc3IRRuntimeSupportLibraryBaseBoolField(
      metadata.runtime_support_library_build_constraints_frozen, out);
  EmitObjc3IRRuntimeSupportLibraryBaseBoolField(
      metadata.runtime_support_library_strict_dispatch_errors_required, out);
  EmitObjc3IRRuntimeSupportLibraryBaseBoolField(
      metadata.runtime_support_library_native_library_present, out);
  EmitObjc3IRRuntimeSupportLibraryBaseBoolField(
      metadata.runtime_support_library_driver_link_wiring_pending, out);
  EmitObjc3IRRuntimeSupportLibraryBaseBoolField(
      metadata.runtime_support_library_ready_for_skeleton, out);
}
