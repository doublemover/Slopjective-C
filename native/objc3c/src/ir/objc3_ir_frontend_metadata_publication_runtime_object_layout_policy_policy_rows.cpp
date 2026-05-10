#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy_policy_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy_row_helpers.h"
#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"

void EmitObjc3IRRuntimeLayoutPolicyOrderingFields(
    const Objc3RuntimeMetadataLayoutPolicy &runtime_metadata_layout_policy,
    std::ostringstream &out) {
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.family_ordering_model, out);
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.descriptor_ordering_model, out);
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.aggregate_relocation_policy, out);
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.comdat_policy, out);
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.visibility_spelling_policy, out);
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.retention_ordering_model, out);
}

void EmitObjc3IRRuntimeLayoutPolicyObjectFormatFields(
    const Objc3RuntimeMetadataLayoutPolicy &runtime_metadata_layout_policy,
    std::ostringstream &out) {
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.object_format_policy_model, out);
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.object_format_surface_contract_id, out);
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.object_format, out);
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.section_spelling_model, out);
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.retention_anchor_model, out);
}
