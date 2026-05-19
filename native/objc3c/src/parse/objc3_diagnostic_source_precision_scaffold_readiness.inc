bool IsObjc3ParserDiagnosticSourcePrecisionScaffoldReady(
    const Objc3ParserDiagnosticSourcePrecisionScaffold &scaffold) {
  return scaffold.scaffold_consistent && !scaffold.scaffold_key.empty();
}
