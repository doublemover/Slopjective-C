#pragma once

#include <filesystem>
#include <string>

bool WriteFrontendCApiRunnerSummaryFile(const std::filesystem::path &summary_path,
                                        const std::string &summary_json,
                                        std::string &error);
