template <typename SiteMetadata>
static void AccumulateConcurrencyReplayRaceGuardSummaryFromSiteMetadata(
    const SiteMetadata &metadata,
    Objc3ConcurrencyReplayRaceGuardSummary &summary) {
  const std::size_t aggregate_sites =
      std::max({metadata.concurrency_replay_race_guard_sites,
                metadata.concurrency_replay_sites,
                metadata.replay_proof_sites,
                metadata.race_guard_sites,
                metadata.task_handoff_sites,
                metadata.actor_isolation_sites});
  summary.concurrency_replay_race_guard_sites += aggregate_sites;
  summary.concurrency_replay_sites += metadata.concurrency_replay_sites;
  summary.replay_proof_sites += metadata.replay_proof_sites;
  summary.race_guard_sites += metadata.race_guard_sites;
  summary.task_handoff_sites += metadata.task_handoff_sites;
  summary.actor_isolation_sites += metadata.actor_isolation_sites;
  summary.deterministic_schedule_sites += metadata.deterministic_schedule_sites;
  summary.guard_blocked_sites += metadata.concurrency_replay_guard_blocked_sites;
  summary.contract_violation_sites +=
      metadata.concurrency_replay_contract_violation_sites;
  summary.deterministic =
      summary.deterministic &&
      metadata.deterministic_concurrency_replay_race_guard_handoff &&
      (aggregate_sites == 0u ||
       metadata.concurrency_replay_race_guard_profile_is_normalized);
}

static void FinalizeConcurrencyReplayRaceGuardSummaryDeterminism(
    Objc3ConcurrencyReplayRaceGuardSummary &summary) {
  summary.deterministic =
      summary.deterministic &&
      summary.concurrency_replay_sites <=
          summary.concurrency_replay_race_guard_sites &&
      summary.replay_proof_sites <=
          summary.concurrency_replay_race_guard_sites &&
      summary.race_guard_sites <= summary.concurrency_replay_race_guard_sites &&
      summary.task_handoff_sites <= summary.concurrency_replay_race_guard_sites &&
      summary.actor_isolation_sites <=
          summary.concurrency_replay_race_guard_sites &&
      summary.deterministic_schedule_sites <= summary.concurrency_replay_sites &&
      summary.guard_blocked_sites <= summary.concurrency_replay_sites &&
      summary.contract_violation_sites <=
          summary.concurrency_replay_race_guard_sites &&
      summary.deterministic_schedule_sites + summary.guard_blocked_sites ==
          summary.concurrency_replay_sites;
}

static Objc3ConcurrencyReplayRaceGuardSummary
BuildConcurrencyReplayRaceGuardSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3ConcurrencyReplayRaceGuardSummary summary;
  for (const auto &entry : surface.functions) {
    AccumulateConcurrencyReplayRaceGuardSummaryFromSiteMetadata(entry.second,
                                                                summary);
  }
  for (const auto &interface_entry : surface.interfaces) {
    for (const auto &method_entry : interface_entry.second.methods) {
      AccumulateConcurrencyReplayRaceGuardSummaryFromSiteMetadata(
          method_entry.second, summary);
    }
  }
  for (const auto &implementation_entry : surface.implementations) {
    for (const auto &method_entry : implementation_entry.second.methods) {
      AccumulateConcurrencyReplayRaceGuardSummaryFromSiteMetadata(
          method_entry.second, summary);
    }
  }
  FinalizeConcurrencyReplayRaceGuardSummaryDeterminism(summary);
  return summary;
}

static Objc3ConcurrencyReplayRaceGuardSummary
BuildConcurrencyReplayRaceGuardSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3ConcurrencyReplayRaceGuardSummary summary;
  for (const auto &metadata : handoff.functions_lexicographic) {
    AccumulateConcurrencyReplayRaceGuardSummaryFromSiteMetadata(metadata,
                                                                summary);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata :
         interface_metadata.methods_lexicographic) {
      AccumulateConcurrencyReplayRaceGuardSummaryFromSiteMetadata(
          method_metadata, summary);
    }
  }
  for (const auto &implementation_metadata :
       handoff.implementations_lexicographic) {
    for (const auto &method_metadata :
         implementation_metadata.methods_lexicographic) {
      AccumulateConcurrencyReplayRaceGuardSummaryFromSiteMetadata(
          method_metadata, summary);
    }
  }
  FinalizeConcurrencyReplayRaceGuardSummaryDeterminism(summary);
  return summary;
}
