Objc3InlineAsmIntrinsicGovernanceProfile
BuildInlineAsmIntrinsicGovernanceProfileFromFunction(const FunctionDecl &fn) {
  const Objc3InlineAsmIntrinsicSiteCounts counts =
      CountInlineAsmIntrinsicSitesInBody(fn.body);
  return BuildInlineAsmIntrinsicGovernanceProfileFromCounts(
      counts.inline_asm_sites,
      counts.intrinsic_sites,
      counts.governed_intrinsic_sites,
      counts.privileged_intrinsic_sites);
}
