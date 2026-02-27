#ifndef OBJC3C_PIPELINE_FRONTEND_PIPELINE_CONTRACT_H_
#define OBJC3C_PIPELINE_FRONTEND_PIPELINE_CONTRACT_H_

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace objc3c::pipeline {

inline constexpr std::uint32_t kFrontendPipelineContractVersionMajor = 1;
inline constexpr std::uint32_t kFrontendPipelineContractVersionMinor = 0;
inline constexpr std::uint32_t kFrontendPipelineContractVersionPatch = 0;

inline constexpr std::size_t kRuntimeDispatchDefaultArgs = 4;
inline constexpr std::size_t kRuntimeDispatchMaxArgs = 16;
inline constexpr const char *kRuntimeDispatchDefaultSymbol = "objc3_msgsend_i32";

enum class StageId : std::uint8_t {
  Lex = 0,
  Parse = 1,
  Sema = 2,
  Lower = 3,
  Emit = 4,
};

inline constexpr std::array<StageId, 5> kStageOrder = {
    StageId::Lex,
    StageId::Parse,
    StageId::Sema,
    StageId::Lower,
    StageId::Emit,
};

enum class StageStatus : std::uint8_t {
  NotRun = 0,
  Succeeded = 1,
  Failed = 2,
  Skipped = 3,
};

enum class StageSkipReason : std::uint8_t {
  None = 0,
  UpstreamFailure = 1,
  InvalidInput = 2,
  UnsupportedMode = 3,
};

enum class ErrorPropagationModel : std::uint8_t {
  NoThrowFailClosed = 0,
};

enum class DiagnosticSeverity : std::uint8_t {
  Note = 0,
  Warning = 1,
  Error = 2,
  Fatal = 3,
};

struct DiagnosticRecord {
  DiagnosticSeverity severity = DiagnosticSeverity::Error;
  std::string code;
  std::string message;
  std::uint32_t line = 0;
  std::uint32_t column = 0;
};

struct DiagnosticsEnvelope {
  StageId stage = StageId::Lex;
  std::vector<DiagnosticRecord> diagnostics;
  std::size_t note_count = 0;
  std::size_t warning_count = 0;
  std::size_t error_count = 0;
  std::size_t fatal_count = 0;
  bool has_error = false;
  bool has_fatal = false;
};

struct StageResult {
  StageId stage = StageId::Lex;
  StageStatus status = StageStatus::NotRun;
  StageSkipReason skip_reason = StageSkipReason::None;
  bool no_throw = true;
  bool fail_closed = true;
  DiagnosticsEnvelope diagnostics;
  std::string failure_reason;
};

struct FrontendPipelineInput {
  std::string source_path;
  std::string source_text;
  std::string emit_prefix = "module";
  std::string output_dir = "artifacts/compilation/objc3c-native";
  std::string clang_path = "clang";
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

#endif  // OBJC3C_PIPELINE_FRONTEND_PIPELINE_CONTRACT_H_
