#include "parser/frontend/objc3_parser_type_projection_internal.h"

namespace objc3c::parse {

void CopyObjc3MethodReturnTypeUnsafeProjection(const FunctionDecl &source,
                                               Objc3MethodDecl &target) {
  target.unsafe_pointer_extension_profile_is_normalized =
      source.unsafe_pointer_extension_profile_is_normalized;
  target.deterministic_unsafe_pointer_extension_handoff =
      source.deterministic_unsafe_pointer_extension_handoff;
  target.unsafe_pointer_extension_sites = source.unsafe_pointer_extension_sites;
  target.unsafe_keyword_sites = source.unsafe_keyword_sites;
  target.pointer_arithmetic_sites = source.pointer_arithmetic_sites;
  target.raw_pointer_type_sites = source.raw_pointer_type_sites;
  target.unsafe_operation_sites = source.unsafe_operation_sites;
  target.unsafe_pointer_extension_normalized_sites =
      source.unsafe_pointer_extension_normalized_sites;
  target.unsafe_pointer_extension_gate_blocked_sites =
      source.unsafe_pointer_extension_gate_blocked_sites;
  target.unsafe_pointer_extension_contract_violation_sites =
      source.unsafe_pointer_extension_contract_violation_sites;
  target.unsafe_pointer_extension_profile = source.unsafe_pointer_extension_profile;
  target.inline_asm_intrinsic_governance_profile_is_normalized =
      source.inline_asm_intrinsic_governance_profile_is_normalized;
  target.deterministic_inline_asm_intrinsic_governance_handoff =
      source.deterministic_inline_asm_intrinsic_governance_handoff;
  target.inline_asm_intrinsic_sites = source.inline_asm_intrinsic_sites;
  target.inline_asm_sites = source.inline_asm_sites;
  target.intrinsic_sites = source.intrinsic_sites;
  target.governed_intrinsic_sites = source.governed_intrinsic_sites;
  target.privileged_intrinsic_sites = source.privileged_intrinsic_sites;
  target.inline_asm_intrinsic_normalized_sites =
      source.inline_asm_intrinsic_normalized_sites;
  target.inline_asm_intrinsic_gate_blocked_sites =
      source.inline_asm_intrinsic_gate_blocked_sites;
  target.inline_asm_intrinsic_contract_violation_sites =
      source.inline_asm_intrinsic_contract_violation_sites;
  target.inline_asm_intrinsic_governance_profile =
      source.inline_asm_intrinsic_governance_profile;
}

}  // namespace objc3c::parse
