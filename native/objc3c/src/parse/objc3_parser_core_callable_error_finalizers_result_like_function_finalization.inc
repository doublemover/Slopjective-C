  void FinalizeResultLikeProfile(FunctionDecl &fn) {
    const Objc3ResultLikeProfile profile =
        BuildResultLikeProfileFromBody(fn.body);
    PublishResultLikeProfileSiteCounters(fn, profile);
    RebuildResultLikeProfile(fn);
    PublishResultLikeProfileNormalizationStatus(fn);
  }
