#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy_state_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy_row_helpers.h"
#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"

void EmitObjc3IRRuntimeLayoutPolicyStateFields(
    const Objc3RuntimeMetadataLayoutPolicy &runtime_metadata_layout_policy,
    std::ostringstream &out) {
  EmitObjc3IRRuntimeLayoutPolicyBoolField(runtime_metadata_layout_policy.ready,
                                          out);
  EmitObjc3IRRuntimeLayoutPolicyBoolField(
      runtime_metadata_layout_policy.fail_closed, out);
}
