inline std::uint64_t MixObjc3DraftSyntaxSurfaceFingerprintInteropReadiness(
    std::uint64_t fingerprint,
    const Objc3DraftSyntaxSurfaceSummary &summary) {
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_attribute_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_import_module_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_swift_annotation_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_cxx_annotation_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_header_import_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_header_export_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_abi_alignment_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_foreign_type_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_mixed_image_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_package_entry_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.interop_error_bridge_sites));
  return fingerprint;
}
