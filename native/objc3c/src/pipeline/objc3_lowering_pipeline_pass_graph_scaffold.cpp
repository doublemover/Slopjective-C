#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold_owners.h"

Objc3LoweringPipelinePassGraphScaffold BuildObjc3LoweringPipelinePassGraphScaffold(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  Objc3LoweringPipelinePassGraphScaffold scaffold;
  objc3_lowering_pipeline_pass_graph_scaffold::PopulateEvidence(
      scaffold, pipeline_result);
  objc3_lowering_pipeline_pass_graph_scaffold::PublishReadiness(scaffold,
                                                                options);
  objc3_lowering_pipeline_pass_graph_scaffold::PublishFailureReason(scaffold);
  return scaffold;
}
