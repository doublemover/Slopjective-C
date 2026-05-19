Objc3NSErrorBridgingProfile BuildNSErrorBridgingProfileFromFunction(
    const FunctionDecl &fn) {
  return BuildNSErrorBridgingProfileFromParameters(
      fn.params, CountFailableCallSitesInBody(fn.body));
}
