#include "tools/objc3c_frontend_c_api_runner_public_result_json_artifacts.h"

#include <ostream>
#include <sstream>

#include "io/json/json_writer.h"

using objc3::io::json::JsonObjectWriter;

namespace {

std::string RenderFrontendCApiRunnerPathsJson(
    const FrontendCApiRunnerArtifactPathView &paths) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("summary", paths.summary);
  object.StringField("diagnostics", paths.diagnostics);
  object.StringField("manifest", paths.manifest);
  object.StringField("ir", paths.ir);
  object.StringField("object", paths.object);
  object.StringField("runtime_metadata_binary", paths.runtime_metadata_binary);
  object.End();
  return out.str();
}

}  // namespace

void WriteFrontendCApiRunnerPublicResultArtifactJsonRows(
    std::ostream &out,
    const FrontendCApiRunnerPublicResultView &public_result) {
  out << "  \"paths\": " << RenderFrontendCApiRunnerPathsJson(public_result.paths)
      << ",\n";
}
