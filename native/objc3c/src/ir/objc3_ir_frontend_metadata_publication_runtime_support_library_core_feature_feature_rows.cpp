#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_core_feature_feature_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_core_feature_row_helpers.h"

void EmitObjc3IRRuntimeSupportLibraryCoreFeatureStateFields(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureBoolField(
      metadata.runtime_support_library_core_feature_fail_closed, out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureBoolField(
      metadata.runtime_support_library_core_feature_sources_present, out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureBoolField(
      metadata.runtime_support_library_core_feature_header_present, out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureBoolField(
      metadata.runtime_support_library_core_feature_archive_build_enabled,
      out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureBoolField(
      metadata.runtime_support_library_core_feature_entrypoints_implemented,
      out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureBoolField(
      metadata.runtime_support_library_core_feature_selector_lookup_stateful,
      out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureBoolField(
      metadata
          .runtime_support_library_core_feature_dispatch_formula_matches_runtime_test_helper,
      out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureBoolField(
      metadata
          .runtime_support_library_core_feature_reset_for_testing_supported,
      out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureBoolField(
      metadata
          .runtime_support_library_core_feature_strict_dispatch_errors_required,
      out);
}
