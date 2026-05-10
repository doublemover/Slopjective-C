  void FinalizeAwaitSuspensionProfile(Objc3MethodDecl &method) {
    const Objc3AwaitSuspensionProfile profile =
        BuildAwaitSuspensionProfileFromOpaqueBody(method);
    PublishAwaitSuspensionProfileFields(method, profile);
    RebuildAwaitSuspensionProfile(method);
    PublishAwaitSuspensionNormalizationStatus(method);
  }
