void RecordObjc3FrontendArtifactPostPipelineFailure(
    Objc3FrontendArtifactPostPipelineFailure &failure,
    const char *code,
    std::string message) {
  if (failure.present) {
    return;
  }
  failure.present = true;
  failure.code = code == nullptr ? "" : code;
  failure.message = std::move(message);
}

bool FinalizeObjc3FrontendPostPipelineFailure(
    Objc3FrontendArtifactBundle &bundle,
    const Objc3FrontendOptions &options,
    const Objc3FrontendArtifactPostPipelineFailure &failure) {
  if (failure.empty()) {
    return false;
  }

  if (!options.emit_ir && !options.emit_object) {
    return true;
  }

  bundle.post_pipeline_diagnostics = {
      MakeDiag(1, 1, failure.code, failure.message)};
  bundle.diagnostics = bundle.post_pipeline_diagnostics;
  return true;
}
