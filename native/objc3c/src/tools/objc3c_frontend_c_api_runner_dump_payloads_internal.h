#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

void SeedFrontendCApiRunnerSummaryDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const std::string &summary_json);

void AppendFrontendCApiRunnerDumpPayloadPasses(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const std::string &result_error_message,
    const std::string &runtime_metadata_binary_path_text);

void AppendFrontendCApiRunnerObservabilityDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const std::string &result_error_message,
    const std::string &runtime_metadata_binary_path_text);

void AppendFrontendCApiRunnerPlaygroundReproDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result);

void AppendFrontendCApiRunnerRuntimeInspectorDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result);

void AppendFrontendCApiRunnerStageTraceDumpPayload(
    std::vector<std::string> &payloads,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result);
