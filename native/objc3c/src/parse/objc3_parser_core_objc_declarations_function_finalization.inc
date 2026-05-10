  void FinalizeFunctionSemanticProfiles(
      FunctionDecl &fn,
      bool has_return_annotation) {
    FinalizeThrowsDeclarationProfile(fn, has_return_annotation);
    FinalizeResultLikeProfile(fn);
    FinalizeNSErrorBridgingProfile(fn);
    FinalizeErrorBridgeMarkerProfile(fn);
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
