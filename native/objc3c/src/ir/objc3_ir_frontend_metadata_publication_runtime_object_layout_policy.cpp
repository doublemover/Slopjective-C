#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy_identity_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy_policy_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy_replay_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy_retention_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy_state_rows.h"

void EmitObjc3IRRuntimeLayoutPolicyMetadataNode(
    const Objc3RuntimeMetadataLayoutPolicy &runtime_metadata_layout_policy,
    std::ostringstream &out) {
  BeginObjc3IRRuntimeLayoutPolicyMetadataNode(runtime_metadata_layout_policy,
                                              out);
  EmitObjc3IRRuntimeLayoutPolicyStateFields(runtime_metadata_layout_policy,
                                            out);
  EmitObjc3IRRuntimeLayoutPolicyOrderingFields(runtime_metadata_layout_policy,
                                               out);
  EmitObjc3IRRuntimeLayoutPolicyObjectFormatFields(
      runtime_metadata_layout_policy, out);
  EmitObjc3IRRuntimeLayoutPolicyRetentionFields(runtime_metadata_layout_policy,
                                                out);
  EmitObjc3IRRuntimeLayoutPolicyReplayFields(runtime_metadata_layout_policy,
                                             out);
  EndObjc3IRRuntimeLayoutPolicyMetadataNode(out);
}
