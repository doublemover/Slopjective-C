#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_contract_source.h"

#include <ostream>

#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerPlaygroundReproContractSourceJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    const FrontendCApiRunnerArtifactPathView &paths) {
  out << child_indent << "\"contract_id\": "
      << "\"objc3c.playground.repro.surface.v1\",\n";
  out << child_indent << "\"available\": "
      << (result.emit.attempted != 0 ? "true" : "false") << ",\n";
  out << child_indent << "\"source_path\": \""
      << EscapeJsonString(options.input_path.generic_string()) << "\",\n";
  out << child_indent << "\"summary_path\": \""
      << EscapeJsonString(paths.summary) << "\",\n";
  out << child_indent << "\"artifact_root\": "
      << "\"" << EscapeJsonString(options.out_dir.generic_string()) << "\",\n";
}
