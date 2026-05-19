  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.function_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.interface_category_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_category_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.function_prototype_count));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.function_pure_count));
