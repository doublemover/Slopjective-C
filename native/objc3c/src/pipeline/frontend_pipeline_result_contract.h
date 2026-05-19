#pragma once

#include "pipeline/frontend_pipeline_stage_contract.h"
#include "pipeline/frontend_pipeline_stage_inputs.h"
#include "pipeline/frontend_pipeline_stage_outputs.h"

namespace objc3c::pipeline {

struct FrontendPipelineOutput {
  ErrorPropagationModel error_model = ErrorPropagationModel::NoThrowFailClosed;
  StageResult lex;
  StageResult parse;
  StageResult sema;
  StageResult lower;
  StageResult emit;
  LexStageOutput lex_output;
  ParseStageOutput parse_output;
  SemaStageOutput sema_output;
  LowerStageOutput lower_output;
  EmitStageOutput emit_output;
  bool success = false;
  int process_exit_code = 0;
};

struct FrontendPipeline {
  FrontendPipelineInput input;
  FrontendPipelineOutput output;
};

}  // namespace objc3c::pipeline
