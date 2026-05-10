void CollectTaskRuntimeCancellationSitesFromSymbol(
    const std::string &symbol,
    Objc3TaskRuntimeCancellationSiteCounts &counts) {
  if (IsTaskRuntimeHookSymbol(symbol)) {
    counts.runtime_hook_sites += 1u;
  }
  if (IsCancellationCheckSymbol(symbol)) {
    counts.cancellation_check_sites += 1u;
  }
  if (IsCancellationHandlerSymbol(symbol)) {
    counts.cancellation_handler_sites += 1u;
  }
  if (IsSuspensionPointSymbol(symbol)) {
    counts.suspension_point_sites += 1u;
  }
}

void CollectTaskRuntimeCancellationProfileSymbol(
    const std::string &symbol,
    void *context) {
  auto *counts =
      static_cast<Objc3TaskRuntimeCancellationSiteCounts *>(context);
  CollectTaskRuntimeCancellationSitesFromSymbol(symbol, *counts);
}
