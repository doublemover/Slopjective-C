  void FinalizeNSErrorBridgingProfile(Objc3MethodDecl &method) {
    const Objc3NSErrorBridgingProfile profile =
        BuildNSErrorBridgingProfileFromOpaqueBody(method);
    PublishNSErrorBridgingSiteCounters(method, profile);
    RebuildNSErrorBridgingProfile(method);
    PublishNSErrorBridgingNormalizationStatus(method);
  }
