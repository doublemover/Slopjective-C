#include "tools/objc3c_frontend_c_api_runner_playground_repro_json.h"

#include <sstream>

#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_context.h"
#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_sections.h"

void WriteFrontendCApiRunnerPlaygroundReproJson(
    std::ostream &out,
    const std::string &indent,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &summary_path_text) {
  const FrontendCApiRunnerPlaygroundReproContext context =
      BuildFrontendCApiRunnerPlaygroundReproContext(
          indent,
          result,
          summary_path_text);

  out << "{\n";
  WriteFrontendCApiRunnerPlaygroundReproContractSourceProfileSections(
      out,
      context,
      options,
      result);
  WriteFrontendCApiRunnerPlaygroundReproPublicSurfaceSection(
      out,
      context);
  WriteFrontendCApiRunnerPlaygroundReproDumpCommandSection(
      out,
      context,
      options);
  out << indent << "}";
}

std::string BuildFrontendCApiRunnerPlaygroundReproJson(
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    const std::filesystem::path &summary_path) {
  std::ostringstream dump;
  WriteFrontendCApiRunnerPlaygroundReproJson(
      dump,
      "",
      options,
      result,
      summary_path.generic_string());
  dump << "\n";
  return dump.str();
}
