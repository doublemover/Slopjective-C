#include "pipeline/frontend_pipeline_stage_contract.h"

#include <utility>

namespace objc3c::pipeline {

const std::array<StageId, 5> &FrontendPipelineStageOrder() {
  return kStageOrder;
}

const char *StageIdName(StageId stage) {
  switch (stage) {
    case StageId::Lex:
      return "lex";
    case StageId::Parse:
      return "parse";
    case StageId::Sema:
      return "sema";
    case StageId::Lower:
      return "lower";
    case StageId::Emit:
      return "emit";
  }
  return "unknown";
}

const char *StageStatusName(StageStatus status) {
  switch (status) {
    case StageStatus::NotRun:
      return "not-run";
    case StageStatus::Succeeded:
      return "succeeded";
    case StageStatus::Failed:
      return "failed";
    case StageStatus::Skipped:
      return "skipped";
  }
  return "unknown";
}

const char *StageSkipReasonName(StageSkipReason reason) {
  switch (reason) {
    case StageSkipReason::None:
      return "none";
    case StageSkipReason::UpstreamFailure:
      return "upstream-failure";
    case StageSkipReason::InvalidInput:
      return "invalid-input";
    case StageSkipReason::UnsupportedMode:
      return "unsupported-mode";
  }
  return "unknown";
}

const char *DiagnosticSeverityName(DiagnosticSeverity severity) {
  switch (severity) {
    case DiagnosticSeverity::Note:
      return "note";
    case DiagnosticSeverity::Warning:
      return "warning";
    case DiagnosticSeverity::Error:
      return "error";
    case DiagnosticSeverity::Fatal:
      return "fatal";
  }
  return "unknown";
}

bool StageStatusIsTerminal(StageStatus status) {
  return status == StageStatus::Succeeded || status == StageStatus::Failed ||
         status == StageStatus::Skipped;
}

bool StageResultFailed(const StageResult &result) {
  return result.status == StageStatus::Failed || result.diagnostics.has_error ||
         result.diagnostics.has_fatal;
}

bool FrontendPipelineOwnerIsExplicit(const std::string &owner) {
  return !owner.empty() && owner.rfind("native.", 0) == 0;
}

bool FrontendPipelineOwnerModelIsStrict(const std::string &owner_model) {
  return owner_model == kFrontendPipelineNoFallbackOwnerModel;
}

bool StageResultOwnerSplitIsReady(const StageResult &result) {
  return FrontendPipelineOwnerIsExplicit(result.stage_input_owner) &&
         FrontendPipelineOwnerIsExplicit(result.stage_output_owner) &&
         FrontendPipelineOwnerIsExplicit(result.diagnostic_handoff_owner) &&
         FrontendPipelineOwnerModelIsStrict(result.owner_model) &&
         result.strict_no_fallback && result.strict_no_compatibility;
}

std::string StageResultOwnerReplayKey(const StageResult &result) {
  return "stage_input_owner=" + result.stage_input_owner +
         ";stage_output_owner=" + result.stage_output_owner +
         ";diagnostic_handoff_owner=" + result.diagnostic_handoff_owner +
         ";owner_model=" + result.owner_model +
         ";strict_no_fallback=" +
         (result.strict_no_fallback ? "true" : "false") +
         ";strict_no_compatibility=" +
         (result.strict_no_compatibility ? "true" : "false") +
         ";owner_split_explicit=" +
         (result.owner_split_explicit ? "true" : "false");
}

DiagnosticsEnvelope BuildDiagnosticsEnvelope(
    StageId stage,
    std::vector<DiagnosticRecord> diagnostics) {
  DiagnosticsEnvelope envelope;
  envelope.stage = stage;
  envelope.diagnostic_handoff_owner = kFrontendPipelineDiagnosticHandoffOwner;
  envelope.owner_model = kFrontendPipelineNoFallbackOwnerModel;
  envelope.diagnostics = std::move(diagnostics);
  for (const DiagnosticRecord &diagnostic : envelope.diagnostics) {
    switch (diagnostic.severity) {
      case DiagnosticSeverity::Note:
        ++envelope.note_count;
        break;
      case DiagnosticSeverity::Warning:
        ++envelope.warning_count;
        break;
      case DiagnosticSeverity::Error:
        ++envelope.error_count;
        envelope.has_error = true;
        break;
      case DiagnosticSeverity::Fatal:
        ++envelope.fatal_count;
        envelope.has_fatal = true;
        break;
    }
  }
  return envelope;
}

StageResult BuildSkippedStageResult(StageId stage,
                                    StageSkipReason reason,
                                    std::string failure_reason) {
  StageResult result;
  result.stage = stage;
  result.status = StageStatus::Skipped;
  result.skip_reason = reason;
  result.failure_reason = std::move(failure_reason);
  result.diagnostics.stage = stage;
  result.owner_split_explicit = StageResultOwnerSplitIsReady(result);
  result.diagnostics.diagnostic_handoff_owner = result.diagnostic_handoff_owner;
  result.diagnostics.owner_model = result.owner_model;
  return result;
}

}  // namespace objc3c::pipeline
