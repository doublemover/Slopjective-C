void FinalizeObjc3ParserCoreInlineAsmIntrinsicGovernanceProfile(
    Objc3MethodDecl &method) {
  const Objc3InlineAsmIntrinsicGovernanceProfile profile =
      BuildInlineAsmIntrinsicGovernanceProfileFromOpaqueBody(method);
  PublishInlineAsmIntrinsicGovernanceProfile(method, profile);
  PublishInlineAsmIntrinsicGovernanceDiagnostics(method);
}
