#pragma once

#include <cstddef>
#include <string>

#include "pipeline/frontend_pipeline_defaults.h"
#include "pipeline/frontend_pipeline_stage_contract.h"

namespace objc3c::pipeline {

struct FrontendPipelineInput {
  std::string stage_input_owner = kFrontendPipelineStageInputOwner;
  std::string owner_model = kFrontendPipelineNoFallbackOwnerModel;
  std::string source_path;
  std::string source_text;
  std::string emit_prefix = "module";
  std::string output_dir = "tmp/artifacts/compilation/objc3c-native";
  std::string clang_path = "clang";
  std::uint8_t language_version = kFrontendDefaultLanguageVersion;
  LanguageProfile language_profile = LanguageProfile::Canonical;
  std::size_t max_message_send_args = kRuntimeDispatchDefaultArgs;
  std::string runtime_dispatch_symbol = kRuntimeDispatchDefaultSymbol;
};

struct LexStageInput {
  std::string stage_input_owner = kFrontendPipelineStageInputOwner;
  std::string source_text;
};

struct ParseStageInput {
  std::string stage_input_owner = kFrontendPipelineStageInputOwner;
  std::size_t token_count = 0;
};

struct SemaStageInput {
  std::string stage_input_owner = kFrontendPipelineStageInputOwner;
  std::size_t declared_globals = 0;
  std::size_t declared_functions = 0;
  std::size_t max_message_send_args = kRuntimeDispatchDefaultArgs;
};

struct LowerStageInput {
  std::string stage_input_owner = kFrontendPipelineStageInputOwner;
  std::size_t declared_globals = 0;
  std::size_t declared_functions = 0;
  std::size_t runtime_dispatch_arg_slots = kRuntimeDispatchDefaultArgs;
  std::string runtime_dispatch_symbol = kRuntimeDispatchDefaultSymbol;
};

struct EmitStageInput {
  std::string stage_input_owner = kFrontendPipelineStageInputOwner;
  std::string ir_path;
  std::string clang_path = "clang";
};

}  // namespace objc3c::pipeline
