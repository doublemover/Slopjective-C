inline std::uint64_t MixObjc3DraftSyntaxSurfaceFingerprintFinalHandoff(
    std::uint64_t fingerprint,
    const Objc3DraftSyntaxSurfaceSummary &summary) {
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.draft_syntax_surface_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      summary.normalized ? 1ull : 0ull);
  fingerprint = MixObjc3ParserContractFingerprintString(
      fingerprint,
      summary.replay_key);
  return fingerprint;
}
