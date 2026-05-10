Objc3UnwindCleanupProfile BuildUnwindCleanupProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3UnwindCleanupSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectUnwindCleanupProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInOpaqueMethodBody(method, walker);
  return BuildUnwindCleanupProfileFromCounts(
      counts.exceptional_exit_sites,
      counts.cleanup_action_sites,
      counts.cleanup_scope_sites,
      counts.cleanup_resume_sites);
}
