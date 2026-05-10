#pragma once

#include "io/json/json_writer.h"
#include "tools/objc3c_frontend_c_api_runner_public_result_ownership.h"

void WriteFrontendCApiRunnerCOwnershipAggregateLifetimeRows(
    objc3::io::json::JsonObjectWriter &object,
    const FrontendCApiRunnerCOwnershipView &ownership);

void WriteFrontendCApiRunnerCOwnershipStringNullRows(
    objc3::io::json::JsonObjectWriter &object,
    const FrontendCApiRunnerCOwnershipView &ownership);

void WriteFrontendCApiRunnerCOwnershipArtifactSummaryRows(
    objc3::io::json::JsonObjectWriter &object,
    const FrontendCApiRunnerCOwnershipView &ownership);
