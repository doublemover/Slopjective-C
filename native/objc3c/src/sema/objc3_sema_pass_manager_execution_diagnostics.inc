  const Objc3SemaPassDiagnosticsRun diagnostics_run =
      RunObjc3SemaDiagnosticsPasses(input, handoff, result);
  if (!result.deterministic_semantic_diagnostics) {
    return result;
  }
