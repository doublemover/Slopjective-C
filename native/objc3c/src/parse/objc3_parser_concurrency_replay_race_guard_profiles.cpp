#include "parse/objc3_parser_concurrency_replay_race_guard_profiles.h"

#include <algorithm>
#include <memory>
#include <sstream>
#include <vector>

#include "parse/objc3_parser_profile_helpers.h"
#include "parse/objc3_parser_profile_symbol_walk.h"

namespace objc3c::parse {
namespace {

#include "parse/objc3_parser_concurrency_replay_race_guard_site_collection.inc"

Objc3ConcurrencyReplayRaceGuardProfile
BuildConcurrencyReplayRaceGuardProfileFromCounts(
    std::size_t replay_proof_sites,
    std::size_t race_guard_sites,
    std::size_t task_handoff_sites,
    std::size_t actor_isolation_sites) {
  Objc3ConcurrencyReplayRaceGuardProfile profile;
  profile.replay_proof_sites = replay_proof_sites;
  profile.race_guard_sites = race_guard_sites;
  profile.task_handoff_sites = task_handoff_sites;
  profile.actor_isolation_sites = actor_isolation_sites;
  profile.concurrency_replay_sites =
      std::max({profile.replay_proof_sites,
                profile.race_guard_sites,
                profile.task_handoff_sites,
                profile.actor_isolation_sites});
  profile.concurrency_replay_race_guard_sites =
      profile.concurrency_replay_sites;
  profile.guard_blocked_sites =
      std::min(profile.concurrency_replay_sites,
               profile.race_guard_sites / 2u);
  profile.deterministic_schedule_sites =
      profile.concurrency_replay_sites - profile.guard_blocked_sites;
  if (profile.concurrency_replay_race_guard_sites !=
          profile.concurrency_replay_sites ||
      profile.replay_proof_sites > profile.concurrency_replay_sites ||
      profile.race_guard_sites > profile.concurrency_replay_sites ||
      profile.task_handoff_sites > profile.concurrency_replay_sites ||
      profile.actor_isolation_sites > profile.concurrency_replay_sites ||
      profile.deterministic_schedule_sites >
          profile.concurrency_replay_sites ||
      profile.guard_blocked_sites > profile.concurrency_replay_sites ||
      profile.contract_violation_sites > profile.concurrency_replay_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.deterministic_schedule_sites + profile.guard_blocked_sites !=
      profile.concurrency_replay_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.concurrency_replay_sites) {
    profile.contract_violation_sites = profile.concurrency_replay_sites;
  }
  profile.deterministic_concurrency_replay_race_guard_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

}  // namespace

std::string BuildConcurrencyReplayRaceGuardProfile(
    std::size_t concurrency_replay_race_guard_sites,
    std::size_t concurrency_replay_sites,
    std::size_t replay_proof_sites,
    std::size_t race_guard_sites,
    std::size_t task_handoff_sites,
    std::size_t actor_isolation_sites,
    std::size_t deterministic_schedule_sites,
    std::size_t guard_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_concurrency_replay_race_guard_handoff) {
  std::ostringstream out;
  out << "concurrency-replay-race-guard:concurrency_replay_race_guard_sites="
      << concurrency_replay_race_guard_sites
      << ";concurrency_replay_sites=" << concurrency_replay_sites
      << ";replay_proof_sites=" << replay_proof_sites
      << ";race_guard_sites=" << race_guard_sites
      << ";task_handoff_sites=" << task_handoff_sites
      << ";actor_isolation_sites=" << actor_isolation_sites
      << ";deterministic_schedule_sites=" << deterministic_schedule_sites
      << ";guard_blocked_sites=" << guard_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_concurrency_replay_race_guard_handoff="
      << (deterministic_concurrency_replay_race_guard_handoff ? "true"
                                                              : "false");
  return out.str();
}

bool IsConcurrencyReplayRaceGuardProfileNormalized(
    std::size_t concurrency_replay_race_guard_sites,
    std::size_t concurrency_replay_sites,
    std::size_t replay_proof_sites,
    std::size_t race_guard_sites,
    std::size_t task_handoff_sites,
    std::size_t actor_isolation_sites,
    std::size_t deterministic_schedule_sites,
    std::size_t guard_blocked_sites,
    std::size_t contract_violation_sites) {
  if (concurrency_replay_race_guard_sites != concurrency_replay_sites ||
      replay_proof_sites > concurrency_replay_sites ||
      race_guard_sites > concurrency_replay_sites ||
      task_handoff_sites > concurrency_replay_sites ||
      actor_isolation_sites > concurrency_replay_sites ||
      deterministic_schedule_sites > concurrency_replay_sites ||
      guard_blocked_sites > concurrency_replay_sites ||
      contract_violation_sites > concurrency_replay_sites) {
    return false;
  }
  if (deterministic_schedule_sites + guard_blocked_sites !=
      concurrency_replay_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

Objc3ConcurrencyReplayRaceGuardProfile
BuildConcurrencyReplayRaceGuardProfileFromFunction(const FunctionDecl &fn) {
  const Objc3ConcurrencyReplayRaceGuardSiteCounts counts =
      CountConcurrencyReplayRaceGuardSitesInBody(fn.body);
  return BuildConcurrencyReplayRaceGuardProfileFromCounts(
      counts.replay_proof_sites,
      counts.race_guard_sites,
      counts.task_handoff_sites,
      counts.actor_isolation_sites);
}

Objc3ConcurrencyReplayRaceGuardProfile
BuildConcurrencyReplayRaceGuardProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3ConcurrencyReplayRaceGuardSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectConcurrencyReplayRaceGuardProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInOpaqueMethodBody(method, walker);
  return BuildConcurrencyReplayRaceGuardProfileFromCounts(
      counts.replay_proof_sites,
      counts.race_guard_sites,
      counts.task_handoff_sites,
      counts.actor_isolation_sites);
}

}  // namespace objc3c::parse
