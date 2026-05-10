Objc3AsyncContinuationProfile BuildAsyncContinuationProfileFromFunction(
    const FunctionDecl &fn) {
  Objc3AsyncContinuationSiteCounts counts =
      CountAsyncContinuationSitesInBody(fn.body);
  if (fn.async_declared) {
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
