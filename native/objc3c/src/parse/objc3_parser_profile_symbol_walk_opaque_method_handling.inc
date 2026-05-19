void WalkObjc3ProfileSymbolsInOpaqueMethodBody(
    const Objc3MethodDecl &method,
    const Objc3ProfileSymbolWalker &walker) {
  if (method.has_body) {
    walker.Visit(method.selector);
  }
}
