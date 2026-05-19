#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "pipeline/readiness/objc3_parse_lowering_conformance_performance_readiness.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_closeout_readiness.h"
#include "pipeline/readiness/objc3_typed_sema_lowering_readiness.h"

struct Objc3ParseLoweringFailureReasonReadinessRecord {
  bool ready_for_lowering = false;
  std::string failure_reason;
};

Objc3ParseLoweringFailureReasonReadinessRecord
Objc3ParseLoweringFailureReasonReadinessReady(const std::string &failure_reason);

Objc3ParseLoweringFailureReasonReadinessRecord
Objc3ParseLoweringFailureReasonReadinessFailure(const std::string &failure_reason);

Objc3ParseLoweringFailureReasonReadinessRecord
BuildObjc3ParseLoweringFailureReasonReadiness(
    const Objc3ParseLoweringReadinessSurface &surface,
    const Objc3TypedSemaLoweringReadinessRecord &typed_sema_lowering_readiness,
    const Objc3ParseLoweringConformancePerformanceReadinessRecord
        &conformance_performance_readiness,
    const Objc3ToolchainRuntimeGaOperationsCloseoutReadinessRecord
        &toolchain_runtime_ga_operations_closeout_readiness);
