#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_sections.h"

#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_artifacts.h"
#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_compile_profile.h"
#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_contract_source.h"

void WriteFrontendCApiRunnerPlaygroundReproContractSourceProfileSections(
    std::ostream &out,
    const FrontendCApiRunnerPlaygroundReproContext &context,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result) {
  WriteFrontendCApiRunnerPlaygroundReproContractSourceJsonRows(
      out,
      context.child_indent,
      options,
      result,
      context.paths);
  WriteFrontendCApiRunnerPlaygroundReproArtifactPathJsonRows(
      out,
      context.child_indent,
      context.grandchild_indent,
      context.paths);
  WriteFrontendCApiRunnerPlaygroundReproCompileProfileJsonRows(
      out,
      context.child_indent,
      context.grandchild_indent,
      options);
}
