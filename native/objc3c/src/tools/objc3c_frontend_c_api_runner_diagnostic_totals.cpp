#include "tools/objc3c_frontend_c_api_runner_diagnostic_totals.h"

namespace {

void AccumulateStageDiagnostics(
    const objc3c_frontend_c_stage_summary_t &summary,
    FrontendCApiDiagnosticTotals &totals) {
  totals.total += summary.diagnostics_total;
  totals.notes += summary.diagnostics_notes;
  totals.warnings += summary.diagnostics_warnings;
  totals.errors += summary.diagnostics_errors;
  totals.fatals += summary.diagnostics_fatals;
}

}  // namespace

FrontendCApiDiagnosticTotals BuildFrontendCApiDiagnosticTotals(
    const objc3c_frontend_c_compile_result_t &result) {
  FrontendCApiDiagnosticTotals totals;
  AccumulateStageDiagnostics(result.lex, totals);
  AccumulateStageDiagnostics(result.parse, totals);
  AccumulateStageDiagnostics(result.sema, totals);
  AccumulateStageDiagnostics(result.lower, totals);
  AccumulateStageDiagnostics(result.emit, totals);
  return totals;
}

const char *HighestFrontendCApiDiagnosticSeverity(
    const FrontendCApiDiagnosticTotals &totals) {
  if (totals.fatals != 0) {
    return "fatal";
  }
  if (totals.errors != 0) {
    return "error";
  }
  if (totals.warnings != 0) {
    return "warning";
  }
  if (totals.notes != 0) {
    return "note";
  }
  return "none";
}
