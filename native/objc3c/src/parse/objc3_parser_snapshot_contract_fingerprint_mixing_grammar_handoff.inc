  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.draft_syntax_surface_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      snapshot.draft_syntax_surface_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprintString(
      fingerprint,
      snapshot.draft_syntax_surface_handoff_key);
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      snapshot.draft_syntax_surface_handoff_deterministic ? 1ull : 0ull);
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.long_tail_grammar_construct_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.long_tail_grammar_covered_construct_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      snapshot.long_tail_grammar_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprintString(
      fingerprint,
      snapshot.long_tail_grammar_handoff_key);
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      snapshot.long_tail_grammar_handoff_deterministic ? 1ull : 0ull);
