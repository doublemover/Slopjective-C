  void FinalizeErrorDiagnosticsRecoveryProfile(Objc3MethodDecl &method) {
    const Objc3ErrorDiagnosticsRecoveryProfile profile =
        BuildErrorDiagnosticsRecoveryProfileFromOpaqueBody(method);
    PublishErrorDiagnosticsRecoverySiteCounters(method, profile);
    RebuildErrorDiagnosticsRecoveryProfile(method);
    PublishErrorDiagnosticsRecoveryNormalizationStatus(method);
  }
