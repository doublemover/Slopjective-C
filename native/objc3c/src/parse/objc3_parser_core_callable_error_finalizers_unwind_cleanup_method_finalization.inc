  void FinalizeUnwindCleanupProfile(Objc3MethodDecl &method) {
    const Objc3UnwindCleanupProfile profile =
        BuildUnwindCleanupProfileFromOpaqueBody(method);
    PublishUnwindCleanupSiteCounters(method, profile);
    RebuildUnwindCleanupProfile(method);
    PublishUnwindCleanupNormalizationStatus(method);
  }
