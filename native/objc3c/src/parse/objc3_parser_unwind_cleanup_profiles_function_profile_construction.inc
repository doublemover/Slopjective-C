Objc3UnwindCleanupProfile BuildUnwindCleanupProfileFromFunction(
    const FunctionDecl &fn) {
  const Objc3UnwindCleanupSiteCounts counts =
      CountUnwindCleanupSitesInBody(fn.body);
  return BuildUnwindCleanupProfileFromCounts(
      counts.exceptional_exit_sites,
      counts.cleanup_action_sites,
      counts.cleanup_scope_sites,
      counts.cleanup_resume_sites);
}
