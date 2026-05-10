#pragma once

#include <array>
#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"
#include "tools/objc3c_frontend_c_api_runner_string_snapshot.h"

struct FrontendCApiRunnerCStringOwnershipContract {
  bool present = false;
  const char *snapshot_owner = "runner-public-result-snapshot";
  const char *accessor_name = "objc3c_frontend_c_result_error_message";
  const char *storage_owner = "result";
  const char *accessor_view = "borrowed-until-result-destroy";
  const char *release_function = "objc3c_frontend_c_result_destroy";
  const char *null_contract = "absent-payload";
};

struct FrontendCApiRunnerCArtifactOwnershipContract {
  const char *name = "";
  bool required_by_runner = false;
  bool produced = false;
  bool path_snapshot_present = false;
  const char *snapshot_owner = "runner-public-result-snapshot";
  const char *presence_accessor = "objc3c_frontend_c_result_has_artifact";
  const char *path_accessor = "objc3c_frontend_c_result_artifact_path";
  const char *storage_owner = "result";
  const char *accessor_view = "borrowed-until-result-destroy";
  const char *release_function = "objc3c_frontend_c_result_destroy";
  const char *null_contract = "absent-or-not-requested";
};

struct FrontendCApiRunnerCArtifactRequirement {
  objc3c_frontend_c_artifact_kind_t artifact_kind =
      OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS;
  const char *name = "";
  bool required_by_runner = false;
};

struct FrontendCApiRunnerCOwnershipView {
  const char *result_storage_owner = "runner-stack";
  const char *result_release_function = "objc3c_frontend_c_result_destroy";
  const char *result_release_timing = "compile-session-guard-destructor";
  const char *publication_snapshot_owner = "runner-public-result-snapshot";
  const char *context_lifetime = "created-before-compile-destroyed-after-snapshot";
  const char *compile_options_lifetime =
      "borrowed-pointers-owned-by-runner-invocation";
  const char *standalone_string_release_function =
      "objc3c_frontend_c_string_release";
  FrontendCApiRunnerCStringOwnershipContract error_message;
  std::array<FrontendCApiRunnerCArtifactOwnershipContract, 5> artifacts;
};

std::array<FrontendCApiRunnerCArtifactRequirement, 5>
BuildFrontendCApiRunnerCArtifactRequirements(
    const FrontendCApiRunnerOptions &options,
    objc3c_frontend_c_status_t status);

FrontendCApiRunnerCOwnershipView BuildFrontendCApiRunnerCOwnershipView(
    const FrontendCApiRunnerOptions &options,
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const FrontendCApiRunnerStringSnapshot &result_error_message);
