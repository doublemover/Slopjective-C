  void FinalizeNSErrorBridgingProfile(FunctionDecl &fn) {
    const Objc3NSErrorBridgingProfile profile =
        BuildNSErrorBridgingProfileFromFunction(fn);
    PublishNSErrorBridgingSiteCounters(fn, profile);
    RebuildNSErrorBridgingProfile(fn);
    PublishNSErrorBridgingNormalizationStatus(fn);
  }
