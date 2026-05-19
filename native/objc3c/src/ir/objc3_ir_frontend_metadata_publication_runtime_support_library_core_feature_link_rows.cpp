#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_core_feature_link_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_core_feature_row_helpers.h"

void EmitObjc3IRRuntimeSupportLibraryCoreFeatureLinkWiringFields(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureBoolField(
      metadata.runtime_support_library_core_feature_driver_link_wiring_pending,
      out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureBoolField(
      metadata
          .runtime_support_library_core_feature_ready_for_driver_link_wiring,
      out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureStringField(
      metadata.runtime_support_library_core_feature_target_name, out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureStringField(
      metadata.runtime_support_library_core_feature_public_header_path, out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureStringField(
      metadata.runtime_support_library_core_feature_source_root, out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureStringField(
      metadata
          .runtime_support_library_core_feature_implementation_source_path,
      out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureStringField(
      metadata.runtime_support_library_core_feature_library_kind, out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureStringField(
      metadata.runtime_support_library_core_feature_archive_basename, out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureStringField(
      metadata.runtime_support_library_core_feature_archive_relative_path, out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureStringField(
      metadata.runtime_support_library_core_feature_probe_source_path, out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureStringField(
      metadata.runtime_support_library_core_feature_register_image_symbol, out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureStringField(
      metadata.runtime_support_library_core_feature_lookup_selector_symbol, out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureStringField(
      metadata.runtime_support_library_core_feature_dispatch_i32_symbol, out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureStringField(
      metadata.runtime_support_library_core_feature_reset_for_testing_symbol,
      out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureStringField(
      metadata.runtime_support_library_core_feature_driver_link_mode, out);
}
