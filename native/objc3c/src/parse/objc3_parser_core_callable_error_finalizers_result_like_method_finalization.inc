  void FinalizeResultLikeProfile(Objc3MethodDecl &method) {
    const Objc3ResultLikeProfile profile =
        BuildResultLikeProfileFromOpaqueBody(method.has_body);
    PublishResultLikeProfileSiteCounters(method, profile);
    RebuildResultLikeProfile(method);
    PublishResultLikeProfileNormalizationStatus(method);
  }
