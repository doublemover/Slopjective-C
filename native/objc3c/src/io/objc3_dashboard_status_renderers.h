#pragma once

#include <string>

#include "io/objc3_process.h"

std::string RenderDashboardProfiles();
std::string RenderDashboardDependencies();
std::string RenderDashboardArtifacts(
    const Objc3DashboardStatusArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &validation_json,
    const std::string &release_evidence_operation_json);
std::string RenderDashboardBlockers();
std::string RenderDashboardSummary();
std::string RenderDashboardRefresh();
std::string RenderDashboardChangeHistory();
