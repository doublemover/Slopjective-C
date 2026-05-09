#pragma once

#include <string>

// ARC runtime debug instrumentation contracts own private testing snapshots
// and deterministic helper counters without widening the public runtime ABI.
inline constexpr const char *kObjc3RuntimeArcDebugInstrumentationContractId =
    "objc3c.runtime.arc.debug.instrumentation.v1";
inline constexpr const char *kObjc3RuntimeArcDebugInstrumentationDependencyModel =
    "live-helper-runtime-plus-private-bootstrap-internal-debug-snapshots";
inline constexpr const char *kObjc3RuntimeArcDebugInstrumentationCoverageModel =
    "retain-release-autorelease-weak-current-property-and-autoreleasepool-helper-traffic-publishes-deterministic-debug-counters-and-last-value-context";
inline constexpr const char *kObjc3RuntimeArcDebugInstrumentationValidationModel =
    "runtime-probes-and-targeted-arc-fixtures-consume-private-debug-snapshots-without-widening-the-public-runtime-abi";
inline constexpr const char *kObjc3RuntimeArcDebugInstrumentationFailClosedModel =
    "arc-debug-hooks-remain-private-testing-surface-only-and-must-not-claim-broader-runtime-completeness";

std::string Objc3RuntimeArcDebugInstrumentationSummary();
