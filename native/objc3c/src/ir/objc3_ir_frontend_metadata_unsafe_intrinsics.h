#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendUnsafeIntrinsicsMetadata {
  std::string lowering_unsafe_pointer_extension_replay_key;
  std::size_t unsafe_pointer_extension_lowering_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_unsafe_keyword_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_pointer_arithmetic_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_raw_pointer_type_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_unsafe_operation_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_normalized_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_gate_blocked_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_contract_violation_sites = 0;
  bool deterministic_unsafe_pointer_extension_lowering_handoff = false;
  std::string lowering_inline_asm_intrinsic_governance_replay_key;
  std::size_t inline_asm_intrinsic_governance_lowering_sites = 0;
  std::size_t inline_asm_intrinsic_governance_lowering_inline_asm_sites = 0;
  std::size_t inline_asm_intrinsic_governance_lowering_intrinsic_sites = 0;
  std::size_t inline_asm_intrinsic_governance_lowering_governed_intrinsic_sites = 0;
  std::size_t inline_asm_intrinsic_governance_lowering_privileged_intrinsic_sites = 0;
  std::size_t inline_asm_intrinsic_governance_lowering_normalized_sites = 0;
  std::size_t inline_asm_intrinsic_governance_lowering_gate_blocked_sites = 0;
  std::size_t inline_asm_intrinsic_governance_lowering_contract_violation_sites = 0;
  bool deterministic_inline_asm_intrinsic_governance_lowering_handoff = false;
};
