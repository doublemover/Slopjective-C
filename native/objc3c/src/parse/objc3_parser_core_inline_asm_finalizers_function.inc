void FinalizeObjc3ParserCoreInlineAsmIntrinsicGovernanceProfile(
    FunctionDecl &fn) {
  const Objc3InlineAsmIntrinsicGovernanceProfile profile =
      BuildInlineAsmIntrinsicGovernanceProfileFromFunction(fn);
  PublishInlineAsmIntrinsicGovernanceProfile(fn, profile);
  PublishInlineAsmIntrinsicGovernanceDiagnostics(fn);
}
