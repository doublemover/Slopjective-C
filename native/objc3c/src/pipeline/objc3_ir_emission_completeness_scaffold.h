#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

std::string BuildObjc3IREmissionCompletenessScaffoldKey(
    const Objc3IREmissionCompletenessScaffold &scaffold);

Objc3IREmissionCompletenessScaffold BuildObjc3IREmissionCompletenessScaffold(
    const Objc3FrontendPipelineResult &pipeline_result);

bool IsObjc3IREmissionCompletenessScaffoldReady(
    const Objc3IREmissionCompletenessScaffold &scaffold,
    std::string &reason);

bool IsObjc3IREmissionCompletenessCoreFeatureReady(
    const Objc3IREmissionCompletenessScaffold &scaffold,
    std::string &reason);

bool IsObjc3IREmissionCompletenessExpansionReady(
    const Objc3IREmissionCompletenessScaffold &scaffold,
    std::string &reason);

bool IsObjc3IREmissionCompletenessEdgeCaseCompatibilityReady(
    const Objc3IREmissionCompletenessScaffold &scaffold,
    std::string &reason);
