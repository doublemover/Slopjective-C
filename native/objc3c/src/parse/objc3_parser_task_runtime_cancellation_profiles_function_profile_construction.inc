Objc3TaskRuntimeCancellationProfile
BuildTaskRuntimeCancellationProfileFromFunction(const FunctionDecl &fn) {
  const Objc3TaskRuntimeCancellationSiteCounts counts =
      CountTaskRuntimeCancellationSitesInBody(fn.body);
  return BuildTaskRuntimeCancellationProfileFromCounts(
      counts.runtime_hook_sites,
      counts.cancellation_check_sites,
      counts.cancellation_handler_sites,
      counts.suspension_point_sites);
}
