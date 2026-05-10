static void DiagnoseConcurrencyActorMethodIsolationRules(
    const Objc3MethodDecl &method, bool owner_is_actor, unsigned line,
    unsigned column, std::vector<std::string> &diagnostics) {
  if (method.objc_nonisolated_declared && !owner_is_actor) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S286",
        "actor isolation semantics failed: objc_nonisolated is only valid on actor methods"));
  }
  if (!owner_is_actor) {
    return;
  }
  const Objc3ConcurrencyActorMethodBodyProfile body_profile =
      BuildConcurrencyActorMethodBodyProfile(method);
  if (method.objc_nonisolated_declared && method.async_declared) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S287",
        "actor isolation semantics failed: objc_nonisolated actor methods cannot also be async"));
  }
  if (method.objc_nonisolated_declared && method.executor_affinity_declared) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S288",
        "actor isolation semantics failed: objc_nonisolated actor methods cannot declare objc_executor affinity"));
  }
  if (body_profile.actor_hop_sites > 0u && !method.async_declared) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S289",
        "actor isolation semantics failed: actor hop sites require an async actor method"));
  }
  if (body_profile.non_sendable_crossing_sites > 0u) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S290",
        "actor sendability semantics failed: non-sendable cross-actor crossings remain unsupported in actor methods"));
  }
}
