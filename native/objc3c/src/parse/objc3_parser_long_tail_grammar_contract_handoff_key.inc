inline std::string BuildObjc3LongTailGrammarHandoffKey(
    const Objc3ParserContractSnapshot &snapshot,
    const bool handoff_deterministic) {
  return "long-tail-grammar:v1:constructs=" +
         std::to_string(snapshot.long_tail_grammar_construct_count) + ";covered=" +
         std::to_string(snapshot.long_tail_grammar_covered_construct_count) + ";fingerprint=" +
         std::to_string(snapshot.long_tail_grammar_fingerprint) + ";ast_shape_fingerprint=" +
         std::to_string(snapshot.ast_shape_fingerprint) + ";ast_top_level_layout_fingerprint=" +
         std::to_string(snapshot.ast_top_level_layout_fingerprint) + ";parser_diagnostics=" +
         std::to_string(snapshot.parser_diagnostic_count) + ";deterministic=" +
         (handoff_deterministic ? "true" : "false");
}
