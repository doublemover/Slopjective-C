namespace {

class PostPipelineFailureRecorder {
 public:
  void Record(const char *code, std::string message) {
    RecordObjc3FrontendArtifactPostPipelineFailure(failure_, code,
                                                   std::move(message));
  }

  [[nodiscard]] const Objc3FrontendArtifactPostPipelineFailure &failure() const {
    return failure_;
  }

 private:
  Objc3FrontendArtifactPostPipelineFailure failure_;
};

}  // namespace
