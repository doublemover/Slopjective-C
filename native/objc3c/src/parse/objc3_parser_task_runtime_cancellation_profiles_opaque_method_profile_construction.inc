Objc3TaskRuntimeCancellationProfile
BuildTaskRuntimeCancellationProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3TaskRuntimeCancellationSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectTaskRuntimeCancellationProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInOpaqueMethodBody(method, walker);
  return BuildTaskRuntimeCancellationProfileFromCounts(
      counts.runtime_hook_sites,
      counts.cancellation_check_sites,
      counts.cancellation_handler_sites,
      counts.suspension_point_sites);
}
