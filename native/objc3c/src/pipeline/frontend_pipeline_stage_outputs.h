#pragma once

#include <cstddef>
#include <string>

#include "lower/contracts/lowering_ir_handoff.h"
#include "pipeline/frontend_pipeline_defaults.h"
#include "pipeline/frontend_pipeline_stage_contract.h"

namespace objc3c::pipeline {

struct LexStageOutput {
  std::string stage_output_owner = kFrontendPipelineStageOutputOwner;
  std::size_t token_count = 0;
  bool eof_token_present = false;
};

struct ParseStageOutput {
  std::string stage_output_owner = kFrontendPipelineStageOutputOwner;
  std::size_t ast_node_count = 0;
  std::size_t declared_globals = 0;
  std::size_t declared_functions = 0;
  bool module_declaration_present = false;
};

struct FunctionSignatureSurface {
  std::size_t scalar_return_i32 = 0;
  std::size_t scalar_return_bool = 0;
  std::size_t scalar_return_void = 0;
  std::size_t scalar_param_i32 = 0;
  std::size_t scalar_param_bool = 0;
};

struct SemaStageOutput {
  std::string stage_output_owner = kFrontendPipelineStageOutputOwner;
  std::string typed_semantic_handoff_owner = "native.frontend.sema.typed-handoff";
  bool semantic_surface_built = false;
  bool semantic_skipped = false;
  std::size_t resolved_global_symbols = 0;
  std::size_t resolved_function_symbols = 0;
  FunctionSignatureSurface function_signature_surface;
};

struct LowerStageOutput {
  std::string stage_output_owner = kFrontendPipelineStageOutputOwner;
  std::string runtime_dispatch_lowering_owner = "native.lower.runtime-dispatch";
  std::string lower_to_ir_handoff_owner = kObjc3LoweringIRHandoffOwner;
  std::string ir_artifact_owner = kObjc3IRModuleArtifactOwner;
  std::string runtime_dispatch_result_owner =
      kObjc3IRRuntimeDispatchResultOwner;
  std::string owner_model = kObjc3LoweringNoFallbackOwnerModel;
  Objc3LoweringIRHandoff lower_to_ir_handoff;
  bool ir_emitted = false;
  bool lower_to_ir_handoff_ready = false;
  bool typed_dispatch_result_ownership_ready = false;
  std::string ir_path;
  std::string runtime_dispatch_symbol = kRuntimeDispatchDefaultSymbol;
  std::size_t runtime_dispatch_arg_slots = kRuntimeDispatchDefaultArgs;
  std::string selector_global_ordering = "lexicographic";
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
};

struct EmitStageOutput {
  std::string stage_output_owner = kFrontendPipelineStageOutputOwner;
  std::string backend_handoff_owner = kFrontendPipelineBackendHandoffOwner;
  bool diagnostics_written = false;
  bool manifest_written = false;
  bool object_written = false;
  std::string diagnostics_path;
  std::string manifest_path;
  std::string object_path;
  int compiler_exit_code = 0;
};

}  // namespace objc3c::pipeline
