  void FinalizeErrorDiagnosticsRecoveryProfile(FunctionDecl &fn) {
    const Objc3ErrorDiagnosticsRecoveryProfile profile =
        BuildErrorDiagnosticsRecoveryProfileFromFunction(fn);
    PublishErrorDiagnosticsRecoverySiteCounters(fn, profile);
    RebuildErrorDiagnosticsRecoveryProfile(fn);
    PublishErrorDiagnosticsRecoveryNormalizationStatus(fn);
  }
