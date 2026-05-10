template <typename DeclT>
void PublishInlineAsmIntrinsicGovernanceDiagnostics(DeclT &target) {
  target.inline_asm_intrinsic_governance_profile =
      BuildInlineAsmIntrinsicGovernanceProfile(
          target.inline_asm_intrinsic_sites,
          target.inline_asm_sites,
          target.intrinsic_sites,
          target.governed_intrinsic_sites,
          target.privileged_intrinsic_sites,
          target.inline_asm_intrinsic_normalized_sites,
          target.inline_asm_intrinsic_gate_blocked_sites,
          target.inline_asm_intrinsic_contract_violation_sites,
          target.deterministic_inline_asm_intrinsic_governance_handoff);
  target.inline_asm_intrinsic_governance_profile_is_normalized =
      IsInlineAsmIntrinsicGovernanceProfileNormalized(
          target.inline_asm_intrinsic_sites,
          target.inline_asm_sites,
          target.intrinsic_sites,
          target.governed_intrinsic_sites,
          target.privileged_intrinsic_sites,
          target.inline_asm_intrinsic_normalized_sites,
          target.inline_asm_intrinsic_gate_blocked_sites,
          target.inline_asm_intrinsic_contract_violation_sites);
}
