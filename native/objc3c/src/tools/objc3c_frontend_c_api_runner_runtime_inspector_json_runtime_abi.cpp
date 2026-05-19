#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_runtime_abi.h"

#include <ostream>

#include "ast/objc3_ast.h"
#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

namespace {

constexpr const char *kObjc3RuntimeArcDebugStateSnapshotSymbol =
    "objc3_runtime_copy_arc_debug_state_for_testing";

}  // namespace

void WriteFrontendCApiRunnerRuntimeInspectorRuntimeAbiJsonRows(
    std::ostream &out,
    const std::string &child_indent) {
  out << child_indent << "\"arc_debug_state_snapshot_symbol\": \""
      << kObjc3RuntimeArcDebugStateSnapshotSymbol << "\",\n";
  out << child_indent
      << "\"object_model_private_snapshot_boundary\": "
         "\"runtime-owned-private-testing-snapshots-no-public-reflection-abi\",\n";
  out << child_indent
      << "\"object_identity_model\": "
         "\"stable-base-identity-plus-instance-and-class-receiver-identities\",\n";
  out << child_indent
      << "\"instance_allocation_model\": "
         "\"runtime-owned-allocation-ordinal-and-zero-initialized-storage-record\",\n";
  out << child_indent
      << "\"dispatch_cache_observability_model\": "
         "\"strict-cache-hit-validation-with-stale-diagnostic-and-snapshots\",\n";
  out << child_indent << "\"runtime_abi_boundary_model\": \""
      << EscapeJsonString(kObjc3RuntimeBlockArcRuntimeAbiBoundaryModel)
      << "\",\n";
  out << child_indent << "\"block_runtime_model\": \""
      << EscapeJsonString(kObjc3RuntimeBlockArcRuntimeAbiBlockModel)
      << "\",\n";
  out << child_indent << "\"arc_runtime_model\": \""
      << EscapeJsonString(kObjc3RuntimeBlockArcRuntimeAbiArcModel) << "\",\n";
  out << child_indent << "\"fail_closed_model\": \""
      << EscapeJsonString(kObjc3RuntimeBlockArcRuntimeAbiFailClosedModel)
      << "\",\n";
}
