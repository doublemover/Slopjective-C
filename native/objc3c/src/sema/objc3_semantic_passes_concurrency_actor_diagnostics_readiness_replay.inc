static void DiagnoseConcurrencyActorRaceHazardRules(
    const Objc3MethodDecl &method, bool owner_is_actor, unsigned line,
    unsigned column, std::vector<std::string> &diagnostics) {
  if (!owner_is_actor) {
    return;
  }
  const Objc3ConcurrencyActorMethodBodyProfile body_profile =
      BuildConcurrencyActorMethodBodyProfile(method);
  if (body_profile.task_handoff_sites == 0u) {
    return;
  }
  if (body_profile.race_guard_sites == 0u) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S291",
        "actor race-hazard semantics failed: task handoff in actor methods requires a race guard site"));
  }
  if (body_profile.replay_proof_sites == 0u) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S292",
        "actor race-hazard semantics failed: task handoff in actor methods requires replay proof coverage"));
  }
  if (body_profile.actor_isolation_sites == 0u) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S293",
        "actor race-hazard semantics failed: task handoff in actor methods requires actor isolation coverage"));
  }
  if (body_profile.escaping_block_literal_sites > 0u) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S294",
        "actor escape semantics failed: escaping block literals remain unsupported in actor methods that perform task handoff"));
  }
}
