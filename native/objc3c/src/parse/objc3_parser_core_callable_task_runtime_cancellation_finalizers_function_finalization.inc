  void FinalizeTaskRuntimeCancellationProfile(FunctionDecl &fn) {
    const Objc3TaskRuntimeCancellationProfile profile =
        BuildTaskRuntimeCancellationProfileFromFunction(fn);
    PublishTaskRuntimeCancellationSiteCounters(fn, profile);
    RebuildTaskRuntimeCancellationProfile(fn);
    PublishTaskRuntimeCancellationNormalizationStatus(fn);
  }
