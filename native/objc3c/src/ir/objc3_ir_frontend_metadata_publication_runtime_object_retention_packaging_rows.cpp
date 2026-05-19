#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_retention_packaging_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_retention_row_helpers.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRRuntimeObjectPackagingRetentionMetadataNode(
    std::ostringstream &out) {
  BeginObjc3IRRuntimeObjectRetentionMetadataRow(
      "!61", kObjc3RuntimeObjectPackagingRetentionContractId, out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      kObjc3RuntimeObjectPackagingRetentionBoundaryModel, out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      kObjc3RuntimeObjectPackagingRetentionAnchorModel, out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      kObjc3RuntimeObjectPackagingRetentionArtifact, out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      kObjc3RuntimeObjectPackagingRetentionSymbolPrefix, out);
  EndObjc3IRRuntimeObjectRetentionMetadataRow(out);
}
