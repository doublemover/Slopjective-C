#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_safety_governance.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRSafetyGovernanceLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!37 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.unsafe_pointer_extension_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.unsafe_pointer_extension_lowering_unsafe_keyword_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.unsafe_pointer_extension_lowering_pointer_arithmetic_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.unsafe_pointer_extension_lowering_raw_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.unsafe_pointer_extension_lowering_unsafe_operation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.unsafe_pointer_extension_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.unsafe_pointer_extension_lowering_gate_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.unsafe_pointer_extension_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_unsafe_pointer_extension_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!38 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.inline_asm_intrinsic_governance_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.inline_asm_intrinsic_governance_lowering_inline_asm_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.inline_asm_intrinsic_governance_lowering_intrinsic_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.inline_asm_intrinsic_governance_lowering_governed_intrinsic_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.inline_asm_intrinsic_governance_lowering_privileged_intrinsic_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.inline_asm_intrinsic_governance_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.inline_asm_intrinsic_governance_lowering_gate_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.inline_asm_intrinsic_governance_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_inline_asm_intrinsic_governance_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
