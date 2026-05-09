#include "tools/objc3c_frontend_c_api_runner_summary_io.h"

#include <fstream>
#include <system_error>

bool WriteFrontendCApiRunnerSummaryFile(const std::filesystem::path &summary_path,
                                        const std::string &summary_json,
                                        std::string &error) {
  std::error_code mkdir_error;
  if (!summary_path.parent_path().empty()) {
    std::filesystem::create_directories(summary_path.parent_path(),
                                        mkdir_error);
  }
  if (mkdir_error) {
    error = "failed to create summary directory '" +
            summary_path.parent_path().string() + "': " +
            mkdir_error.message();
    return false;
  }

  std::ofstream out(summary_path, std::ios::binary);
  if (!out.is_open()) {
    error = "failed to open summary file '" + summary_path.string() +
            "' for writing";
    return false;
  }
  out << summary_json;
  if (!out.good()) {
    error = "failed while writing summary file '" + summary_path.string() + "'";
    return false;
  }
  return true;
}
