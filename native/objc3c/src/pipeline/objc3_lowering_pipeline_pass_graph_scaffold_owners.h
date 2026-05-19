#pragma once

#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold.h"

namespace objc3_lowering_pipeline_pass_graph_scaffold {

void PopulateEvidence(Objc3LoweringPipelinePassGraphScaffold &scaffold,
                      const Objc3FrontendPipelineResult &pipeline_result);

void PublishReadiness(Objc3LoweringPipelinePassGraphScaffold &scaffold,
                      const Objc3FrontendOptions &options);

void PublishFailureReason(Objc3LoweringPipelinePassGraphScaffold &scaffold);

}  // namespace objc3_lowering_pipeline_pass_graph_scaffold
