#pragma once

#include <string>

#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_closeout_readiness.h"

void ApplyObjc3ToolchainRuntimeGaOperationsDocsRunbookSyncCloseoutReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    Objc3ToolchainRuntimeGaOperationsCloseoutReadinessRecord &record,
    bool toolchain_runtime_ga_operations_cross_lane_integration_consistent,
    bool toolchain_runtime_ga_operations_cross_lane_integration_ready,
    std::string &toolchain_runtime_ga_operations_docs_runbook_sync_key);

void ApplyObjc3ToolchainRuntimeGaOperationsAdvancedCoreCloseoutReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    Objc3ToolchainRuntimeGaOperationsCloseoutReadinessRecord &record,
    const std::string &toolchain_runtime_ga_operations_docs_runbook_sync_key,
    std::string &toolchain_runtime_ga_operations_advanced_core_key);
