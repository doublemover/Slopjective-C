  void FinalizeAwaitSuspensionProfile(FunctionDecl &fn) {
    const Objc3AwaitSuspensionProfile profile =
        BuildAwaitSuspensionProfileFromFunction(fn);
    PublishAwaitSuspensionProfileFields(fn, profile);
    RebuildAwaitSuspensionProfile(fn);
    PublishAwaitSuspensionNormalizationStatus(fn);
  }
