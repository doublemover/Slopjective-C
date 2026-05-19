#include "tools/objc3c_frontend_c_api_runner_observability_json_diagnostics.h"

#include <ostream>

void WriteFrontendCApiRunnerObservabilityDiagnosticTotalJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const FrontendCApiDiagnosticTotals &diagnostic_totals,
    bool result_error_message_present) {
  out << child_indent << "\"highest_diagnostic_severity\": \""
      << HighestFrontendCApiDiagnosticSeverity(diagnostic_totals) << "\",\n";
  out << child_indent << "\"result_error_message_present\": "
      << (result_error_message_present ? "true" : "false") << ",\n";
  out << child_indent << "\"diagnostics_total\": " << diagnostic_totals.total
      << ",\n";
  out << child_indent << "\"diagnostics_notes\": " << diagnostic_totals.notes
      << ",\n";
  out << child_indent << "\"diagnostics_warnings\": "
      << diagnostic_totals.warnings << ",\n";
  out << child_indent << "\"diagnostics_errors\": " << diagnostic_totals.errors
      << ",\n";
  out << child_indent << "\"diagnostics_fatals\": " << diagnostic_totals.fatals
      << ",\n";
}
