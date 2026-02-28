#include "libobjc3c_frontend/objc3_cli_frontend.h"

#include <string>

#include "pipeline/objc3_frontend_pipeline.h"

Objc3FrontendArtifactBundle CompileObjc3SourceForCli(const std::filesystem::path &input_path,
                                                     const std::string &source,
                                                     const Objc3FrontendOptions &options) {
  const Objc3FrontendPipelineResult pipeline_result = RunObjc3FrontendPipeline(source, options);
  return BuildObjc3FrontendArtifacts(input_path, pipeline_result, options);
}
