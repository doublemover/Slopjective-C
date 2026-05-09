#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace objc3c::pipeline {

inline constexpr std::uint32_t kFrontendPipelineContractVersionMajor = 1;
inline constexpr std::uint32_t kFrontendPipelineContractVersionMinor = 0;
inline constexpr std::uint32_t kFrontendPipelineContractVersionPatch = 0;
inline constexpr const char *kFrontendPipelineOwnerSplitContractId =
    "objc3c.frontend.pipeline.owner-split.hard-cutover.v1";
inline constexpr const char *kFrontendPipelineStageInputOwner =
    "native.frontend.pipeline.stage-input";
inline constexpr const char *kFrontendPipelineStageOutputOwner =
    "native.frontend.pipeline.stage-output";
inline constexpr const char *kFrontendPipelineDiagnosticHandoffOwner =
    "native.frontend.pipeline.diagnostic-handoff";
inline constexpr const char *kFrontendPipelineBackendHandoffOwner =
    "native.frontend.pipeline.backend-handoff";
inline constexpr const char *kFrontendPipelineNoFallbackOwnerModel =
    "strict-hard-cutover-no-fallback-no-compatibility-shim";

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
  std::string diagnostic_handoff_owner = kFrontendPipelineDiagnosticHandoffOwner;
  std::string owner_model = kFrontendPipelineNoFallbackOwnerModel;
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
  std::string stage_input_owner = kFrontendPipelineStageInputOwner;
  std::string stage_output_owner = kFrontendPipelineStageOutputOwner;
  std::string diagnostic_handoff_owner = kFrontendPipelineDiagnosticHandoffOwner;
  std::string owner_model = kFrontendPipelineNoFallbackOwnerModel;
  bool owner_split_explicit = false;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  DiagnosticsEnvelope diagnostics;
  std::string failure_reason;
};

[[nodiscard]] const std::array<StageId, 5> &FrontendPipelineStageOrder();
[[nodiscard]] const char *StageIdName(StageId stage);
[[nodiscard]] const char *StageStatusName(StageStatus status);
[[nodiscard]] const char *StageSkipReasonName(StageSkipReason reason);
[[nodiscard]] const char *DiagnosticSeverityName(DiagnosticSeverity severity);
[[nodiscard]] bool StageStatusIsTerminal(StageStatus status);
[[nodiscard]] bool StageResultFailed(const StageResult &result);
[[nodiscard]] bool FrontendPipelineOwnerIsExplicit(const std::string &owner);
[[nodiscard]] bool FrontendPipelineOwnerModelIsStrict(
    const std::string &owner_model);
[[nodiscard]] bool StageResultOwnerSplitIsReady(const StageResult &result);
[[nodiscard]] std::string StageResultOwnerReplayKey(const StageResult &result);
[[nodiscard]] DiagnosticsEnvelope BuildDiagnosticsEnvelope(
    StageId stage,
    std::vector<DiagnosticRecord> diagnostics);
[[nodiscard]] StageResult BuildSkippedStageResult(
    StageId stage,
    StageSkipReason reason,
    std::string failure_reason);

}  // namespace objc3c::pipeline
