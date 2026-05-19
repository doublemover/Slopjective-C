  void FinalizeUnsafePointerExtensionProfile(FunctionDecl &fn) {
    const Objc3UnsafePointerExtensionProfile profile =
        BuildUnsafePointerExtensionProfileFromFunction(fn);
    PublishUnsafePointerExtensionProfile(fn, profile);
    RebuildUnsafePointerExtensionProfile(fn);
    PublishUnsafePointerExtensionNormalizationStatus(fn);
  }
