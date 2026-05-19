#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_retention_link_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_retention_row_helpers.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRRuntimeObjectLinkerRetentionMetadataNode(
    const std::string &runtime_metadata_linker_anchor_symbol,
    const std::string &runtime_metadata_discovery_root_symbol,
    std::ostringstream &out) {
  BeginObjc3IRRuntimeObjectRetentionMetadataRow(
      "!62", kObjc3RuntimeLinkerRetentionContractId, out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      kObjc3RuntimeLinkerRetentionAnchorModel, out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      kObjc3RuntimeLinkerDiscoveryModel, out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      kObjc3RuntimeLinkerAnchorLogicalSection, out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      kObjc3RuntimeLinkerDiscoveryRootLogicalSection, out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      runtime_metadata_linker_anchor_symbol, out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      runtime_metadata_discovery_root_symbol, out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      kObjc3RuntimeLinkerResponseArtifactSuffix, out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      kObjc3RuntimeLinkerDiscoveryArtifactSuffix, out);
  EndObjc3IRRuntimeObjectRetentionMetadataRow(out);
}
