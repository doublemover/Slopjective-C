inline std::uint64_t
MixObjc3DraftSyntaxSurfaceFingerprintMacroPropertyDiagnostics(
    std::uint64_t fingerprint,
    const Objc3DraftSyntaxSurfaceSummary &summary) {
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.macro_attribute_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.macro_package_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.macro_provenance_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.macro_cache_key_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.macro_sandbox_policy_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.property_behavior_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.property_attribute_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.property_accessor_selector_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.property_synthesis_metadata_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.property_reflection_input_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(
          summary.property_ownership_nullability_sites));
  return fingerprint;
}
