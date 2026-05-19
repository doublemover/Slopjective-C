#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy_identity_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy_row_helpers.h"
#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"

void BeginObjc3IRRuntimeLayoutPolicyMetadataNode(
    const Objc3RuntimeMetadataLayoutPolicy &runtime_metadata_layout_policy,
    std::ostringstream &out) {
  BeginObjc3IRRuntimeLayoutPolicyMetadataRow(
      "!55", runtime_metadata_layout_policy.contract_id, out);
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.abi_contract_id, out);
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.scaffold_contract_id, out);
}

void EndObjc3IRRuntimeLayoutPolicyMetadataNode(std::ostringstream &out) {
  EndObjc3IRRuntimeLayoutPolicyMetadataRow(out);
}
