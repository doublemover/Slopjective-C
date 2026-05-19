  void FinalizeActorIsolationSendabilityProfile(FunctionDecl &fn) {
    const Objc3ActorIsolationSendabilityProfile profile =
        BuildActorIsolationSendabilityProfileFromFunction(fn);
    PublishActorIsolationSendabilityProfile(fn, profile);
  }
