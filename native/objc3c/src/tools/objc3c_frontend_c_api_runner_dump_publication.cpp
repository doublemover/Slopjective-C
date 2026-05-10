#include "tools/objc3c_frontend_c_api_runner_dump_publication.h"

FrontendCApiRunnerDumpPublication BuildFrontendCApiRunnerDumpPublication(
    const std::filesystem::path &summary_path,
    const FrontendCApiRunnerSessionResult &session_result) {
  FrontendCApiRunnerDumpPublication publication;
  publication.compile_result = session_result.compile_result;
  publication.status = session_result.status;
  publication.summary_path = summary_path;
  publication.result_error_message =
      session_result.public_result.diagnostics.result_error_message;
  publication.runtime_metadata_binary_path_text =
      session_result.artifact_paths.runtime_metadata_binary;
  publication.summary_json = session_result.json;
  return publication;
}

const objc3c_frontend_c_compile_result_t &FrontendCApiRunnerDumpCompileResult(
    const FrontendCApiRunnerDumpPublication &publication) {
  return *publication.compile_result;
}
