  bool ParseOptionalContainerDispatchAttributes(Objc3InterfaceDecl &decl) {
    return objc3c::parse::ParseObjc3OptionalContainerDispatchAttributes(
        tokens_, index_, diagnostics_, decl);
  }

  template <typename TCallableDecl>
  bool ParseOptionalCallableBridgeAttributes(TCallableDecl &decl) {
    return objc3c::parse::ParseObjc3OptionalCallableBridgeAttributes(
        tokens_, index_, diagnostics_, decl);
  }

  template <typename TCallableDecl>
  void FinalizeErrorBridgeMarkerProfile(TCallableDecl &decl) {
    objc3c::parse::FinalizeObjc3ErrorBridgeMarkerProfile(decl);
  }
