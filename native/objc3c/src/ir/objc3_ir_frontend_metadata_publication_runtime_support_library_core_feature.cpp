#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_core_feature.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_core_feature_feature_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_core_feature_library_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_core_feature_link_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_core_feature_row_helpers.h"

void EmitObjc3IRRuntimeSupportLibraryCoreFeatureMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRRuntimeSupportLibraryCoreFeatureMetadataRow(
      "!52", metadata.runtime_support_library_core_feature_contract_id, out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureLibraryFields(metadata, out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureStateFields(metadata, out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureLinkWiringFields(metadata, out);
  EndObjc3IRRuntimeSupportLibraryCoreFeatureMetadataRow(out);
}
