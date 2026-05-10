#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy_replay_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy_row_helpers.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"
#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"

void EmitObjc3IRRuntimeLayoutPolicyReplayFields(
    const Objc3RuntimeMetadataLayoutPolicy &runtime_metadata_layout_policy,
    std::ostringstream &out) {
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      Objc3RuntimeMetadataLayoutPolicyReplayKey(runtime_metadata_layout_policy),
      out);
  EmitObjc3IRRuntimeLayoutPolicyStringField(
      runtime_metadata_layout_policy.failure_reason, out);
}
