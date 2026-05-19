  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.parser_diagnostic_count));
  fingerprint = MixObjc3ParserContractFingerprintString(
      fingerprint,
      snapshot.diagnostic_owner_contract_id);
  fingerprint = MixObjc3ParserContractFingerprintString(
      fingerprint,
      snapshot.diagnostic_stage_owner);
  fingerprint = MixObjc3ParserContractFingerprintString(
      fingerprint,
      snapshot.diagnostic_fixit_owner);
  fingerprint = MixObjc3ParserContractFingerprintString(
      fingerprint,
      snapshot.diagnostic_recovery_owner);
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      snapshot.diagnostic_owner_split_explicit ? 1ull : 0ull);
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      snapshot.diagnostic_recovery_counts_as_success ? 1ull : 0ull);
