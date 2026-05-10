#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_async_diagnostic.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRAsyncDiagnosticLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!42 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_await_keyword_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_await_suspension_point_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_await_resume_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_await_state_machine_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_await_continuation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_gate_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.await_lowering_suspension_state_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_await_lowering_suspension_state_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!43 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_async_keyword_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_async_function_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_continuation_allocation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_continuation_resume_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_continuation_suspend_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_async_state_machine_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_gate_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.async_continuation_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_async_continuation_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!44 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_parser_diagnostic_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_semantic_diagnostic_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_fixit_hint_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_recovery_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_recovery_applied_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.error_diagnostics_recovery_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_error_diagnostics_recovery_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
