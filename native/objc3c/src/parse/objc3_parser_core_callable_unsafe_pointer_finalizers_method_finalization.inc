  void FinalizeUnsafePointerExtensionProfile(Objc3MethodDecl &method) {
    const Objc3UnsafePointerExtensionProfile profile =
        BuildUnsafePointerExtensionProfileFromOpaqueBody(method);
    PublishUnsafePointerExtensionProfile(method, profile);
    RebuildUnsafePointerExtensionProfile(method);
    PublishUnsafePointerExtensionNormalizationStatus(method);
  }
