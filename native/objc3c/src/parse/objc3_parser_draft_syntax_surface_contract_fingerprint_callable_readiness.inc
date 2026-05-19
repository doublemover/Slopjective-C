inline std::uint64_t MixObjc3DraftSyntaxSurfaceFingerprintCallableReadiness(
    std::uint64_t fingerprint,
    const Objc3DraftSyntaxSurfaceSummary &summary) {
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.throws_callable_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.async_callable_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.await_expression_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.actor_interface_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.actor_nonisolated_callable_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.actor_isolation_marker_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.actor_sendable_annotation_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.task_runtime_construct_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.task_cancellation_check_sites));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(summary.task_suspension_point_sites));
  return fingerprint;
}
