template <typename DeclT>
void PublishInlineAsmIntrinsicGovernanceProfile(
    DeclT &target,
    const Objc3InlineAsmIntrinsicGovernanceProfile &profile) {
  target.inline_asm_intrinsic_sites = profile.inline_asm_intrinsic_sites;
  target.inline_asm_sites = profile.inline_asm_sites;
  target.intrinsic_sites = profile.intrinsic_sites;
  target.governed_intrinsic_sites = profile.governed_intrinsic_sites;
  target.privileged_intrinsic_sites = profile.privileged_intrinsic_sites;
  target.inline_asm_intrinsic_normalized_sites = profile.normalized_sites;
  target.inline_asm_intrinsic_gate_blocked_sites = profile.gate_blocked_sites;
  target.inline_asm_intrinsic_contract_violation_sites =
      profile.contract_violation_sites;
  target.deterministic_inline_asm_intrinsic_governance_handoff =
      profile.deterministic_inline_asm_intrinsic_governance_handoff;
}
