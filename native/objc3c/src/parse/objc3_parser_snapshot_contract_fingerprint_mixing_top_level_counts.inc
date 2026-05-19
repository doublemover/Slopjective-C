  constexpr std::uint64_t kInitialFingerprint = 1469598103934665603ull;
  std::uint64_t fingerprint = kInitialFingerprint;
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.token_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.top_level_declaration_count));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.global_decl_count));
