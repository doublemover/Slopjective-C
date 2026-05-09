#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold.h"

bool IsObjc3LoweringPipelinePassGraphScaffoldReady(
    const Objc3LoweringPipelinePassGraphScaffold &scaffold,
    std::string &reason) {
  if (scaffold.pass_graph_ready) {
    reason.clear();
    return true;
  }
  reason = scaffold.failure_reason.empty()
               ? "lowering pipeline pass graph scaffold is not ready"
               : scaffold.failure_reason;
  return false;
}
