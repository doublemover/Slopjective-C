  void FinalizeUnwindCleanupProfile(FunctionDecl &fn) {
    const Objc3UnwindCleanupProfile profile =
        BuildUnwindCleanupProfileFromFunction(fn);
    PublishUnwindCleanupSiteCounters(fn, profile);
    RebuildUnwindCleanupProfile(fn);
    PublishUnwindCleanupNormalizationStatus(fn);
  }
