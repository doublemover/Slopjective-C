#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_concurrency_race_guard.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRConcurrencyRaceGuardLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!39 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.concurrency_replay_race_guard_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.concurrency_replay_race_guard_lowering_replay_proof_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.concurrency_replay_race_guard_lowering_race_guard_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.concurrency_replay_race_guard_lowering_task_handoff_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.concurrency_replay_race_guard_lowering_actor_isolation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.concurrency_replay_race_guard_lowering_deterministic_schedule_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.concurrency_replay_race_guard_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.concurrency_replay_race_guard_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_concurrency_replay_race_guard_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
