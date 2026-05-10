  constexpr std::uint64_t kInitialFingerprint = 1469598103934665603ull;
  std::uint64_t fingerprint = kInitialFingerprint;
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.token_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.top_level_declaration_count));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.global_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.protocol_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.protocol_property_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.protocol_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.protocol_class_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.protocol_instance_method_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.interface_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.interface_property_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.interface_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.interface_class_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.interface_instance_method_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.implementation_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_property_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_class_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_instance_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.function_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.interface_category_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_category_decl_count));
  fingerprint =
      MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.function_prototype_count));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, static_cast<std::uint64_t>(snapshot.function_pure_count));
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
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.ast_shape_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.ast_top_level_layout_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.deterministic_handoff ? 1ull : 0ull);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.parser_recovery_replay_ready ? 1ull : 0ull);
  return fingerprint;
