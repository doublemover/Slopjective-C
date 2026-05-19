Objc3AsyncContinuationProfile BuildAsyncContinuationProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3AsyncContinuationSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectAsyncContinuationProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInOpaqueMethodBody(method, walker);
  if (method.async_declared) {
    counts.async_keyword_sites += 1u;
    counts.async_function_sites += 1u;
  }
  return BuildAsyncContinuationProfileFromCounts(
      counts.async_keyword_sites,
      counts.async_function_sites,
      counts.continuation_allocation_sites,
      counts.continuation_resume_sites,
      counts.continuation_suspend_sites,
      counts.async_state_machine_sites);
}
