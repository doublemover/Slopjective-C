  snapshot.long_tail_grammar_construct_count = BuildObjc3LongTailGrammarConstructCount(snapshot);
  snapshot.long_tail_grammar_covered_construct_count = BuildObjc3LongTailGrammarCoveredConstructCount(snapshot);
  snapshot.long_tail_grammar_fingerprint = BuildObjc3LongTailGrammarFingerprint(snapshot);
  const bool long_tail_grammar_counts_consistent =
      snapshot.long_tail_grammar_covered_construct_count <= snapshot.long_tail_grammar_construct_count;
  snapshot.long_tail_grammar_handoff_deterministic =
      snapshot.long_tail_grammar_fingerprint != 0 &&
      long_tail_grammar_counts_consistent &&
      snapshot.deterministic_handoff &&
      snapshot.parser_recovery_replay_ready;
  snapshot.long_tail_grammar_handoff_key = BuildObjc3LongTailGrammarHandoffKey(
      snapshot,
      snapshot.long_tail_grammar_handoff_deterministic);
  snapshot.long_tail_grammar_handoff_deterministic =
      snapshot.long_tail_grammar_handoff_deterministic &&
      !snapshot.long_tail_grammar_handoff_key.empty();
