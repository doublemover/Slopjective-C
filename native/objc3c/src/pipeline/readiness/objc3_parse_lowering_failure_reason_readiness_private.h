#pragma once

#include "pipeline/readiness/objc3_parse_lowering_failure_reason_readiness.h"

namespace objc3_parse_lowering_failure_reason_readiness_detail {

const char *FindLoweringToolchainCloseoutFailureReason(
    const Objc3ParseLoweringReadinessSurface &surface,
    const Objc3ParseLoweringConformancePerformanceReadinessRecord
        &conformance_performance_readiness,
    const Objc3ToolchainRuntimeGaOperationsCloseoutReadinessRecord
        &toolchain_runtime_ga_operations_closeout_readiness);

const char *FindParseArtifactDiagnosticHandoffFailureReason(
    const Objc3ParseLoweringReadinessSurface &surface);

const char *FindParserDiagnosticGrammarHardeningFailureReason(
    const Objc3ParseLoweringReadinessSurface &surface);

const char *FindParseRecoveryConformanceFailureReason(
    const Objc3ParseLoweringReadinessSurface &surface,
    const Objc3ParseLoweringConformancePerformanceReadinessRecord
        &conformance_performance_readiness);

}  // namespace objc3_parse_lowering_failure_reason_readiness_detail
