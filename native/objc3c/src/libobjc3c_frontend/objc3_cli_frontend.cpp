#include "libobjc3c_frontend/objc3_cli_frontend.h"

#include <string>
#include <utility>

Objc3FrontendCompileProduct CompileObjc3SourceWithPipeline(const std::filesystem::path &input_path,
                                                           const std::string &source,
                                                           const Objc3FrontendOptions &options) {
  Objc3FrontendCompileProduct product;
  product.pipeline_result = RunObjc3FrontendPipeline(source, options);
  product.artifact_bundle = BuildObjc3FrontendArtifacts(input_path, product.pipeline_result, options);
  return product;
}

Objc3FrontendArtifactBundle CompileObjc3SourceForCli(const std::filesystem::path &input_path,
                                                     const std::string &source,
                                                     const Objc3FrontendOptions &options) {
  Objc3FrontendCompileProduct product = CompileObjc3SourceWithPipeline(input_path, source, options);
  return std::move(product.artifact_bundle);
}
