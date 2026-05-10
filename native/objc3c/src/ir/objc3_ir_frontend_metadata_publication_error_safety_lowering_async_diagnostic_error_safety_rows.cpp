#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_async_diagnostic_error_safety_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_async_diagnostic_row_helpers.h"

void EmitObjc3IRErrorSafetyDiagnosticRecoveryCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRAsyncDiagnosticLoweringCounterRow(
      "!44",
      {metadata.error_diagnostics_recovery_lowering_sites,
       metadata.error_diagnostics_recovery_lowering_parser_diagnostic_sites,
       metadata.error_diagnostics_recovery_lowering_semantic_diagnostic_sites,
       metadata.error_diagnostics_recovery_lowering_fixit_hint_sites,
       metadata.error_diagnostics_recovery_lowering_recovery_candidate_sites,
       metadata.error_diagnostics_recovery_lowering_recovery_applied_sites,
       metadata.error_diagnostics_recovery_lowering_normalized_sites,
       metadata.error_diagnostics_recovery_lowering_guard_blocked_sites,
       metadata.error_diagnostics_recovery_lowering_contract_violation_sites},
      metadata.deterministic_error_diagnostics_recovery_lowering_handoff, out);
}
