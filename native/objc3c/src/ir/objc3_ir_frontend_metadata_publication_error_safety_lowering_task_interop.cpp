#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_task_interop.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRTaskInteropCancellationLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!40 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_runtime_interop_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_cancellation_probe_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_cancellation_handler_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_runtime_resume_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_runtime_cancel_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.task_runtime_interop_cancellation_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_task_runtime_interop_cancellation_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
