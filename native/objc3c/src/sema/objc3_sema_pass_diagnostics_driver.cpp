#include "sema/objc3_sema_pass_diagnostics_driver.h"

#include <numeric>
#include <vector>

#include "sema/objc3_sema_diagnostic_publication.h"
#include "sema/objc3_sema_diagnostics_bus.h"
#include "sema/objc3_sema_pass_flow_scaffold.h"
#include "sema/objc3_semantic_passes.h"

Objc3SemaPassDiagnosticsRun RunObjc3SemaDiagnosticsPasses(
    const Objc3SemaPassManagerInput &input,
    const Objc3ParserSemaHandoffScaffold &handoff,
    Objc3SemaPassManagerResult &result) {
  Objc3SemaPassDiagnosticsRun run;
  result.executed = true;
  result.sema_pass_flow_summary.language_profile = input.language_profile;
  result.sema_pass_flow_summary.canonical_literal_rejection_total_sites =
      input.canonical_literal_rejection_counts.total_literal_sites();
  result.sema_pass_flow_summary.stage_input_owner = input.stage_input_owner;
  result.sema_pass_flow_summary.typed_semantic_handoff_owner =
      input.typed_semantic_handoff_owner;
  result.sema_pass_flow_summary.diagnostic_handoff_owner =
      input.diagnostics_bus.diagnostic_handoff_owner;
  result.sema_pass_flow_summary.diagnostic_catalog_owner =
      input.diagnostics_bus.diagnostic_catalog_owner;
  result.sema_pass_flow_summary.diagnostic_fixit_owner =
      input.diagnostics_bus.diagnostic_fixit_owner;
  result.sema_pass_flow_summary.diagnostic_recovery_owner =
      input.diagnostics_bus.diagnostic_recovery_owner;
  result.sema_pass_flow_summary.owner_model = input.owner_model;
  result.sema_pass_flow_summary.strict_no_retired_route = input.strict_no_retired_route;
  result.sema_pass_flow_summary.strict_no_compatibility =
      input.strict_no_compatibility;
  result.sema_pass_flow_summary.recovery_counts_as_success =
      input.recovery_counts_as_success ||
      input.diagnostics_bus.recovery_counts_as_success;

  bool deterministic_semantic_diagnostics = handoff.deterministic;
  bool diagnostics_canonicalized = true;
  bool diagnostics_accounting_consistent = true;
  bool diagnostics_bus_publish_consistent =
      Objc3SemaDiagnosticsBusHasHardCutoverOwner(input.diagnostics_bus);
  std::size_t expected_diagnostics_size = 0u;

  for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {
    run.pass_order_matches_contract =
        run.pass_order_matches_contract &&
        run.pass_iteration_count < kObjc3SemaPassOrder.size() &&
        kObjc3SemaPassOrder[run.pass_iteration_count] == pass;
    ++run.pass_iteration_count;

    const std::size_t pass_index = static_cast<std::size_t>(pass);
    MarkObjc3SemaPassExecuted(result.sema_pass_flow_summary, pass);

    std::vector<std::string> pass_diagnostics;
    if (pass == Objc3SemaPassId::BuildIntegrationSurface) {
      result.integration_surface =
          BuildSemanticIntegrationSurface(
              *input.program,
              input.validation_options.allow_source_only_block_literals,
              input.validation_options.allow_source_only_defer_statements,
              input.validation_options.allow_source_only_error_runtime_surface,
              input.validation_options.arc_mode_enabled,
              pass_diagnostics);
    } else if (pass == Objc3SemaPassId::ValidateBodies) {
      ValidateSemanticBodies(
          *input.program,
          result.integration_surface,
          input.validation_options,
          pass_diagnostics);
      RefreshSemanticIntegrationSurfaceAfterBodyValidation(
          *input.program,
          result.integration_surface);
    } else {
      ValidatePureContractSemanticDiagnostics(
          *input.program,
          result.integration_surface.functions,
          pass_diagnostics);
    }

    CanonicalizeObjc3SemaPassDiagnostics(pass_diagnostics);
    const bool pass_diagnostics_canonical =
        AreObjc3SemaPassDiagnosticsCanonical(pass_diagnostics);
    const bool pass_diagnostics_hard_cutover_owned =
        AreObjc3SemaPassDiagnosticsHardCutoverOwned(pass_diagnostics);
    diagnostics_canonicalized =
        diagnostics_canonicalized && pass_diagnostics_canonical &&
        pass_diagnostics_hard_cutover_owned;
    deterministic_semantic_diagnostics =
        deterministic_semantic_diagnostics && pass_diagnostics_canonical &&
        pass_diagnostics_hard_cutover_owned;

    const std::size_t diagnostics_bus_count_before_publish =
        Objc3SemaDiagnosticsBusCount(input.diagnostics_bus);
    result.diagnostics.insert(
        result.diagnostics.end(),
        pass_diagnostics.begin(),
        pass_diagnostics.end());
    expected_diagnostics_size += pass_diagnostics.size();
    diagnostics_accounting_consistent =
        diagnostics_accounting_consistent &&
        result.diagnostics.size() == expected_diagnostics_size;
    PublishObjc3SemaDiagnostics(input.diagnostics_bus, pass_diagnostics);
    const std::size_t diagnostics_bus_count_after_publish =
        Objc3SemaDiagnosticsBusCount(input.diagnostics_bus);
    const bool pass_bus_publish_consistent =
        !Objc3SemaDiagnosticsBusHasSink(input.diagnostics_bus) ||
        diagnostics_bus_count_after_publish ==
            diagnostics_bus_count_before_publish + pass_diagnostics.size();
    diagnostics_bus_publish_consistent =
        diagnostics_bus_publish_consistent && pass_bus_publish_consistent;
    result.diagnostics_after_pass[static_cast<std::size_t>(pass)] =
        result.diagnostics.size();
    result.diagnostics_emitted_by_pass[pass_index] = pass_diagnostics.size();
  }

  const std::size_t diagnostics_emitted_total = std::accumulate(
      result.diagnostics_emitted_by_pass.begin(),
      result.diagnostics_emitted_by_pass.end(),
      static_cast<std::size_t>(0u));
  const bool diagnostics_emission_totals_consistent =
      diagnostics_emitted_total == result.diagnostics.size();
  run.diagnostics_after_pass_monotonic =
      IsMonotonicObjc3SemaDiagnosticsAfterPass(result.diagnostics_after_pass);
  const bool diagnostics_hardening_has_required_budgets =
      diagnostics_bus_publish_consistent &&
      diagnostics_emission_totals_consistent &&
      run.diagnostics_after_pass_monotonic;

  result.diagnostics_accounting_consistent = diagnostics_accounting_consistent;
  result.diagnostics_bus_publish_consistent = diagnostics_bus_publish_consistent;
  result.diagnostics_canonicalized = diagnostics_canonicalized;
  result.diagnostics_hardening_satisfied =
      result.diagnostics_accounting_consistent &&
      result.diagnostics_bus_publish_consistent &&
      result.diagnostics_canonicalized &&
      diagnostics_hardening_has_required_budgets;
  result.deterministic_semantic_diagnostics =
      deterministic_semantic_diagnostics &&
      result.diagnostics_hardening_satisfied;
  return run;
}
