#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy_retention_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy_row_helpers.h"
#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"

void EmitObjc3IRRuntimeLayoutPolicyRetentionFields(
    const Objc3RuntimeMetadataLayoutPolicy &runtime_metadata_layout_policy,
    std::ostringstream &out) {
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.descriptor_linkage, out);
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.aggregate_linkage, out);
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.metadata_visibility, out);
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.retention_root, out);
  EmitObjc3IRRuntimeLayoutPolicySizeField(
      runtime_metadata_layout_policy.total_retained_global_count, out);
}
