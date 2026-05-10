#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_core_feature_library_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_core_feature_row_helpers.h"

void EmitObjc3IRRuntimeSupportLibraryCoreFeatureLibraryFields(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureStringField(
      metadata.runtime_support_library_core_feature_support_library_contract_id,
      out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureStringField(
      metadata
          .runtime_support_library_core_feature_metadata_publication_contract_id,
      out);
}
