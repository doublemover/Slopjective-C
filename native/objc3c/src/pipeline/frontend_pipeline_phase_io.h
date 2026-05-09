#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "pipeline/frontend_pipeline_stage_contract.h"

namespace objc3c::pipeline {

inline constexpr std::size_t kRuntimeDispatchDefaultArgs = 4;
inline constexpr std::size_t kRuntimeDispatchMaxArgs = 16;
inline constexpr const char *kRuntimeDispatchDefaultSymbol =
    "objc3_runtime_dispatch_i32";
inline constexpr std::uint8_t kFrontendDefaultLanguageVersion = 3u;

enum class LanguageProfile : std::uint8_t {
  Canonical = 0,
};

struct FrontendPipelineInput {
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
  std::string source_text;
};

struct LexStageOutput {
  std::size_t token_count = 0;
  bool eof_token_present = false;
};

struct ParseStageInput {
  std::size_t token_count = 0;
};

struct ParseStageOutput {
  std::size_t ast_node_count = 0;
  std::size_t declared_globals = 0;
  std::size_t declared_functions = 0;
  bool module_declaration_present = false;
};

struct SemaStageInput {
  std::size_t declared_globals = 0;
  std::size_t declared_functions = 0;
  std::size_t max_message_send_args = kRuntimeDispatchDefaultArgs;
};

struct FunctionSignatureSurface {
  std::size_t scalar_return_i32 = 0;
  std::size_t scalar_return_bool = 0;
  std::size_t scalar_return_void = 0;
  std::size_t scalar_param_i32 = 0;
  std::size_t scalar_param_bool = 0;
};

struct SemaStageOutput {
  bool semantic_surface_built = false;
  bool semantic_skipped = false;
  std::size_t resolved_global_symbols = 0;
  std::size_t resolved_function_symbols = 0;
  FunctionSignatureSurface function_signature_surface;
};

struct LowerStageInput {
  std::size_t declared_globals = 0;
  std::size_t declared_functions = 0;
  std::size_t runtime_dispatch_arg_slots = kRuntimeDispatchDefaultArgs;
  std::string runtime_dispatch_symbol = kRuntimeDispatchDefaultSymbol;
};

struct LowerStageOutput {
  bool ir_emitted = false;
  std::string ir_path;
  std::string runtime_dispatch_symbol = kRuntimeDispatchDefaultSymbol;
  std::size_t runtime_dispatch_arg_slots = kRuntimeDispatchDefaultArgs;
  std::string selector_global_ordering = "lexicographic";
};

struct EmitStageInput {
  std::string ir_path;
  std::string clang_path = "clang";
};

struct EmitStageOutput {
  bool diagnostics_written = false;
  bool manifest_written = false;
  bool object_written = false;
  std::string diagnostics_path;
  std::string manifest_path;
  std::string object_path;
  int compiler_exit_code = 0;
};

}  // namespace objc3c::pipeline
