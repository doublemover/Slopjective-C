  void FinalizeTaskRuntimeCancellationProfile(Objc3MethodDecl &method) {
    const Objc3TaskRuntimeCancellationProfile profile =
        BuildTaskRuntimeCancellationProfileFromOpaqueBody(method);
    PublishTaskRuntimeCancellationSiteCounters(method, profile);
    RebuildTaskRuntimeCancellationProfile(method);
    PublishTaskRuntimeCancellationNormalizationStatus(method);
  }
