#include "tools/objc3c_frontend_c_api_runner_summary_path.h"

std::filesystem::path BuildFrontendCApiRunnerSummaryPath(
    const FrontendCApiRunnerOptions &options) {
  return options.summary_out.empty()
             ? (options.out_dir / (options.emit_prefix + ".c_api_summary.json"))
             : options.summary_out;
}
