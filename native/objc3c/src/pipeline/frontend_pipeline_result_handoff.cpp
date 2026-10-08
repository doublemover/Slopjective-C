#include "pipeline/frontend_pipeline_result_handoff.h"

#include <utility>

#include "lex/objc3_lexer_contract.h"
#include "pipeline/objc3_frontend_types.h"
#include "pipeline/results/compile_options.h"
#include "sema/objc3_sema_pass_manager.h"

namespace {

Objc3SemaLanguageProfile Objc3SemaLanguageProfileFromFrontendProfile(
    Objc3FrontendLanguageProfile profile) {
  switch (profile) {
    case Objc3FrontendLanguageProfile::kCanonical:
      return Objc3SemaLanguageProfile::Canonical;
    case Objc3FrontendLanguageProfile::kStrict:
      return Objc3SemaLanguageProfile::Strict;
    case Objc3FrontendLanguageProfile::kStrictConcurrency:
      return Objc3SemaLanguageProfile::StrictConcurrency;
  }
  return Objc3SemaLanguageProfile::Canonical;
}

}  // namespace

void CaptureObjc3FrontendCanonicalLiteralRejections(
    Objc3FrontendPipelineResult &result,
    const Objc3LexerCanonicalLiteralRejectionCounts &lexer_counts) {
  result.canonical_literal_rejection_counts.yes_literal_sites =
      lexer_counts.yes_literal_sites;
  result.canonical_literal_rejection_counts.no_literal_sites =
      lexer_counts.no_literal_sites;
  result.canonical_literal_rejection_counts.null_literal_sites =
      lexer_counts.null_literal_sites;
}

void PopulateObjc3FrontendSemaInputHandoff(
    Objc3FrontendPipelineResult &result,
    const Objc3FrontendOptions &options,
    const Objc3SemanticValidationOptions &semantic_options,
    Objc3SemaPassManagerInput &sema_input) {
  sema_input.program = &result.program;
  sema_input.validation_options = semantic_options;
  sema_input.language_profile =
      Objc3SemaLanguageProfileFromFrontendProfile(options.language_profile);
  sema_input.canonical_literal_rejection_counts.yes_literal_sites =
      result.canonical_literal_rejection_counts.yes_literal_sites;
  sema_input.canonical_literal_rejection_counts.no_literal_sites =
      result.canonical_literal_rejection_counts.no_literal_sites;
  sema_input.canonical_literal_rejection_counts.null_literal_sites =
      result.canonical_literal_rejection_counts.null_literal_sites;
  sema_input.diagnostics_bus.diagnostics =
      &result.stage_diagnostics.semantic;
  sema_input.stage_input_owner = kObjc3SemaStageInputOwner;
  sema_input.typed_semantic_handoff_owner = kObjc3SemaTypedSemanticHandoffOwner;
  sema_input.owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  sema_input.strict_no_retired_route = true;
  sema_input.strict_no_compatibility = true;
  sema_input.diagnostics_bus.diagnostic_handoff_owner =
      kObjc3SemaDiagnosticHandoffOwner;
  sema_input.diagnostics_bus.owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  sema_input.diagnostics_bus.strict_no_retired_route = true;
  sema_input.diagnostics_bus.strict_no_compatibility = true;
}

void AdoptObjc3FrontendSemaResult(Objc3FrontendPipelineResult &result,
                                  Objc3SemaPassManagerResult &&sema_result) {
  result.integration_surface = std::move(sema_result.integration_surface);
  result.sema_type_metadata_handoff =
      std::move(sema_result.type_metadata_handoff);
  result.sema_diagnostics_after_pass = sema_result.diagnostics_after_pass;
  result.sema_pass_flow_summary = sema_result.sema_pass_flow_summary;
  result.sema_parity_surface = sema_result.parity_surface;
}
