std::string BuildObjc3ParserSemaHandoffPublicationRecoveryReplayKey(
    const Objc3ParserSemaConformanceMatrix &matrix,
    const Objc3ParserSemaConformanceCorpus &corpus) {
  std::ostringstream recovery_replay_key_stream;
  recovery_replay_key_stream
      << "sema-pass-recovery:v1:"
      << "matrix_recovery_replay_ready="
      << (matrix.parser_recovery_replay_ready ? "true" : "false")
      << ";corpus_recovery_replay_case_present="
      << (corpus.has_recovery_replay_case ? "true" : "false")
      << ";corpus_recovery_replay_case_passed="
      << (corpus.recovery_replay_case_passed ? "true" : "false")
      << ";corpus_required_case_count=" << corpus.required_case_count
      << ";corpus_passed_case_count=" << corpus.passed_case_count
      << ";corpus_failed_case_count=" << corpus.failed_case_count;
  return recovery_replay_key_stream.str();
}

Objc3ParserSemaHandoffPublicationEvidenceRecord
BuildObjc3ParserSemaHandoffPublicationEvidenceRecord(
    const Objc3ParserSemaHandoffScaffold &handoff) {
  Objc3ParserSemaHandoffPublicationEvidenceRecord record;
  const Objc3ParserSemaConformanceMatrix &matrix =
      handoff.parser_sema_conformance_matrix;
  const Objc3ParserSemaConformanceCorpus &corpus =
      handoff.parser_sema_conformance_corpus;
  record.stage_input_owner = handoff.owner_record.stage_input_owner;
  record.parser_sema_contract_handoff_owner =
      handoff.owner_record.parser_sema_contract_handoff_owner;
  record.owner_model = handoff.owner_record.owner_model;
  record.strict_no_retired_route = handoff.owner_record.strict_no_retired_route;
  record.strict_no_compatibility =
      handoff.owner_record.strict_no_compatibility;
  record.conformance_matrix_ready = matrix.deterministic;
  record.conformance_corpus_ready = corpus.deterministic;
  record.parser_recovery_replay_ready = matrix.parser_recovery_replay_ready;
  record.parser_recovery_replay_case_present =
      corpus.has_recovery_replay_case;
  record.parser_recovery_replay_case_passed =
      corpus.recovery_replay_case_passed;
  record.corpus_required_case_count = corpus.required_case_count;
  record.corpus_passed_case_count = corpus.passed_case_count;
  record.corpus_failed_case_count = corpus.failed_case_count;
  record.corpus_case_counts_ready =
      record.corpus_required_case_count > 0u &&
      record.corpus_passed_case_count == record.corpus_required_case_count &&
      record.corpus_failed_case_count == 0u;
  record.parser_recovery_replay_contract_satisfied =
      record.parser_recovery_replay_ready &&
      record.parser_recovery_replay_case_present &&
      record.parser_recovery_replay_case_passed &&
      record.corpus_case_counts_ready;
  record.recovery_replay_key =
      BuildObjc3ParserSemaHandoffPublicationRecoveryReplayKey(matrix, corpus);
  record.recovery_replay_key_deterministic =
      record.conformance_matrix_ready && record.conformance_corpus_ready &&
      !record.recovery_replay_key.empty();
  record.recovery_determinism_hardening_satisfied =
      record.parser_recovery_replay_contract_satisfied &&
      record.recovery_replay_key_deterministic;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.handoff_publication_evidence_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_conformance_evidence_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.conformance_matrix_ready && record.conformance_corpus_ready &&
      record.parser_recovery_replay_ready &&
      record.parser_recovery_replay_case_present &&
      record.parser_recovery_replay_case_passed &&
      record.corpus_case_counts_ready &&
      record.parser_recovery_replay_contract_satisfied &&
      record.recovery_replay_key.rfind("sema-pass-recovery:v1:", 0) == 0 &&
      record.recovery_replay_key_deterministic &&
      record.recovery_determinism_hardening_satisfied;
  return record;
}
