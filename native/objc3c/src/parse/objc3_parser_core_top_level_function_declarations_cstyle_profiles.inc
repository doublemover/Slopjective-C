  void FinalizeTopLevelCStyleFunctionProfiles(FunctionDecl &fn) {
    FinalizeThrowsDeclarationProfile(fn, true);
    FinalizeResultLikeProfile(fn);
    FinalizeNSErrorBridgingProfile(fn);
    FinalizeUnwindCleanupProfile(fn);
    FinalizeErrorDiagnosticsRecoveryProfile(fn);
    FinalizeAsyncContinuationProfile(fn);
    FinalizeAwaitSuspensionProfile(fn);
    FinalizeActorIsolationSendabilityProfile(fn);
    FinalizeTaskRuntimeCancellationProfile(fn);
    FinalizeConcurrencyReplayRaceGuardProfile(fn);
    FinalizeUnsafePointerExtensionProfile(fn);
    FinalizeInlineAsmIntrinsicGovernanceProfile(fn);
  }
