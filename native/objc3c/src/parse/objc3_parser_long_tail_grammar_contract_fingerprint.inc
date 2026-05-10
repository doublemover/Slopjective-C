inline std::uint64_t BuildObjc3LongTailGrammarFingerprint(const Objc3ParserContractSnapshot &snapshot) {
  constexpr std::uint64_t kInitialFingerprint = 1469598103934665603ull;
  std::uint64_t fingerprint = kInitialFingerprint;
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.long_tail_grammar_construct_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.long_tail_grammar_covered_construct_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.protocol_property_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.protocol_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.interface_property_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.interface_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_property_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_method_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.interface_category_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.implementation_category_decl_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.function_prototype_count));
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.function_pure_count));
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.ast_shape_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.ast_top_level_layout_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprint(fingerprint, snapshot.draft_syntax_surface_fingerprint);
  fingerprint = MixObjc3ParserContractFingerprint(
      fingerprint,
      static_cast<std::uint64_t>(snapshot.parser_diagnostic_count));
  return fingerprint;
}
