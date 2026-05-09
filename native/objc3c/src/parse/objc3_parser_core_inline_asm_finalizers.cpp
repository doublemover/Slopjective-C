#include "parse/objc3_parser_core_inline_asm_finalizers.h"

#include "parse/objc3_parser_inline_asm_intrinsic_profiles.h"

namespace objc3c::parse {

void FinalizeObjc3ParserCoreInlineAsmIntrinsicGovernanceProfile(
    FunctionDecl &fn) {
  const Objc3InlineAsmIntrinsicGovernanceProfile profile =
      BuildInlineAsmIntrinsicGovernanceProfileFromFunction(fn);
  fn.inline_asm_intrinsic_sites = profile.inline_asm_intrinsic_sites;
  fn.inline_asm_sites = profile.inline_asm_sites;
  fn.intrinsic_sites = profile.intrinsic_sites;
  fn.governed_intrinsic_sites = profile.governed_intrinsic_sites;
  fn.privileged_intrinsic_sites = profile.privileged_intrinsic_sites;
  fn.inline_asm_intrinsic_normalized_sites = profile.normalized_sites;
  fn.inline_asm_intrinsic_gate_blocked_sites = profile.gate_blocked_sites;
  fn.inline_asm_intrinsic_contract_violation_sites =
      profile.contract_violation_sites;
  fn.deterministic_inline_asm_intrinsic_governance_handoff =
      profile.deterministic_inline_asm_intrinsic_governance_handoff;
  fn.inline_asm_intrinsic_governance_profile =
      BuildInlineAsmIntrinsicGovernanceProfile(
          fn.inline_asm_intrinsic_sites,
          fn.inline_asm_sites,
          fn.intrinsic_sites,
          fn.governed_intrinsic_sites,
          fn.privileged_intrinsic_sites,
          fn.inline_asm_intrinsic_normalized_sites,
          fn.inline_asm_intrinsic_gate_blocked_sites,
          fn.inline_asm_intrinsic_contract_violation_sites,
          fn.deterministic_inline_asm_intrinsic_governance_handoff);
  fn.inline_asm_intrinsic_governance_profile_is_normalized =
      IsInlineAsmIntrinsicGovernanceProfileNormalized(
          fn.inline_asm_intrinsic_sites,
          fn.inline_asm_sites,
          fn.intrinsic_sites,
          fn.governed_intrinsic_sites,
          fn.privileged_intrinsic_sites,
          fn.inline_asm_intrinsic_normalized_sites,
          fn.inline_asm_intrinsic_gate_blocked_sites,
          fn.inline_asm_intrinsic_contract_violation_sites);
}

void FinalizeObjc3ParserCoreInlineAsmIntrinsicGovernanceProfile(
    Objc3MethodDecl &method) {
  const Objc3InlineAsmIntrinsicGovernanceProfile profile =
      BuildInlineAsmIntrinsicGovernanceProfileFromOpaqueBody(method);
  method.inline_asm_intrinsic_sites = profile.inline_asm_intrinsic_sites;
  method.inline_asm_sites = profile.inline_asm_sites;
  method.intrinsic_sites = profile.intrinsic_sites;
  method.governed_intrinsic_sites = profile.governed_intrinsic_sites;
  method.privileged_intrinsic_sites = profile.privileged_intrinsic_sites;
  method.inline_asm_intrinsic_normalized_sites = profile.normalized_sites;
  method.inline_asm_intrinsic_gate_blocked_sites = profile.gate_blocked_sites;
  method.inline_asm_intrinsic_contract_violation_sites =
      profile.contract_violation_sites;
  method.deterministic_inline_asm_intrinsic_governance_handoff =
      profile.deterministic_inline_asm_intrinsic_governance_handoff;
  method.inline_asm_intrinsic_governance_profile =
      BuildInlineAsmIntrinsicGovernanceProfile(
          method.inline_asm_intrinsic_sites,
          method.inline_asm_sites,
          method.intrinsic_sites,
          method.governed_intrinsic_sites,
          method.privileged_intrinsic_sites,
          method.inline_asm_intrinsic_normalized_sites,
          method.inline_asm_intrinsic_gate_blocked_sites,
          method.inline_asm_intrinsic_contract_violation_sites,
          method.deterministic_inline_asm_intrinsic_governance_handoff);
  method.inline_asm_intrinsic_governance_profile_is_normalized =
      IsInlineAsmIntrinsicGovernanceProfileNormalized(
          method.inline_asm_intrinsic_sites,
          method.inline_asm_sites,
          method.intrinsic_sites,
          method.governed_intrinsic_sites,
          method.privileged_intrinsic_sites,
          method.inline_asm_intrinsic_normalized_sites,
          method.inline_asm_intrinsic_gate_blocked_sites,
          method.inline_asm_intrinsic_contract_violation_sites);
}

}  // namespace objc3c::parse
