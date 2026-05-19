#pragma once
#include "io/objc3_process.h"

#include "io/json/json_parser.h"
#include "io/json/json_writer.h"
#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process_boundary_tokens.h"
#include "io/objc3_process_json_helpers.h"
#include "io/objc3_json.h"
#include "io/objc3_object_format.h"
#include "lower/objc3_lowering_contract.h"
#include "support/objc3_identifier_safe_suffix.h"

#if defined(_WIN32)
#include <process.h>
#else
#include <spawn.h>
#include <sys/wait.h>
#endif

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>


using objc3::io::EscapeJsonString;
using objc3::io::json::JsonObjectWriter;
using objc3::io::json::JsonValue;
#if !defined(_WIN32)
extern char **environ;
#endif

// live task runtime anchor: packaged task-runtime probes continue to
// use the existing runtime library path and linked execution surface; no
// separate public scheduler/runtime package is introduced here.
// hardening anchor: replay/hardening probes for the same task
// runtime continue to package against that existing runtime archive path.
// system-helper/runtime-contract anchor: Part 8 runtime/helper proof
// likewise stays on the same packaged runtime archive path. The driver/process
// layer does not introduce a separate cleanup/resource runtime package or a
// dedicated borrowed-pointer import surface for this freeze; there is still no
// dedicated borrowed-pointer import surface here.
// live cleanup/runtime integration anchor: the linked Part 8 runtime
// probe also packages against that same runtime archive path plus the emitted
// module object; there is still no separate resource-runtime package boundary.
// bridge-packaging/toolchain anchor: Part 11 now freezes the same
// packaged runtime archive and sidecar topology as the truthful toolchain-
// visible interop boundary. The process layer validates imported Part 11
// preservation packets through the mixed-module link plan, but it still does
// not claim live header/module/bridge generation here.

inline constexpr const char *kObjc3RuntimeBootstrapTableConsumptionContractId =
    "objc3c.runtime.bootstrap.table.consumption.freeze.v1";
inline constexpr const char *kObjc3RuntimeBootstrapTableConsumptionModel =
    "next-public-register-call-consumes-staged-registration-table-once";
inline constexpr const char *kObjc3RuntimeBootstrapTableDeduplicationModel =
    "translation-unit-identity-key-rejection-before-registration-state-advance";
inline constexpr const char
    *kObjc3RuntimeBootstrapTableImageStatePublicationModel =
        "image-walk-snapshot-publishes-module-identity-root-counts-and-staged-table-usage";
inline constexpr const char *kObjc3RuntimeLiveRegistrationDiscoveryReplayContractId =
    "objc3c.runtime.live.registration.discovery.replay.v1";
inline constexpr const char *kObjc3RuntimeLiveRegistrationModel =
    "emitted-metadata-images-register-through-native-runtime-and-retained-bootstrap-catalog";
inline constexpr const char *kObjc3RuntimeLiveDiscoveryTrackingModel =
    "image-walk-snapshot-tracks-last-discovered-root-and-descriptor-families";
inline constexpr const char *kObjc3RuntimeLiveReplayTrackingModel =
    "reset-replay-state-snapshot-tracks-retained-images-reset-clears-and-last-replayed-identity";
inline constexpr const char *kObjc3RuntimeLiveRestartHardeningContractId =
    "objc3c.runtime.live.restart.hardening.v1";
inline constexpr const char *kObjc3RuntimeLiveIdempotenceModel =
    "second-live-replay-without-reset-fails-closed-and-preserves-live-runtime-state";
inline constexpr const char *kObjc3RuntimeLiveTeardownModel =
    "reset-clears-live-state-zeroes-image-local-init-cells-and-retains-bootstrap-catalog";
inline constexpr const char *kObjc3RuntimeLiveRestartEvidenceModel =
    "repeated-reset-replay-cycles-publish-monotonic-reset-and-replay-generations";
inline constexpr const char *kObjc3ToolchainConformanceClaimOperationsContractId =
    "objc3c.toolchain.conformance.claim.operations.v1";
inline constexpr const char *kObjc3ToolchainConformanceClaimValidationSchemaId =
    "objc3c-driver-conformance-validation-v1";
inline constexpr const char *kObjc3ToolchainConformanceClaimValidationModel =
    "driver-validates-versioned-conformance-report-and-publication-sidecars-before-toolchain-consumption";
inline constexpr const char *kObjc3ToolchainConformanceClaimConsumptionModel =
    "validation-consumes-json-sidecars-only-and-keeps-unsupported-profiles-fail-closed";
inline constexpr const char *kObjc3AdvancedFeatureOpsContractId =
    "objc3c.advanced.feature.ci.runbook.dashboard.contract.v1";
inline constexpr const char *kObjc3AdvancedFeatureReportingContractId =
    "objc3c.tooling.feature.aware.conformance.report.emission.v1";
inline constexpr const char *kObjc3AdvancedFeatureReleaseEvidenceContractId =
    "objc3c.tooling.corpus.sharding.release.evidence.packaging.v1";
inline constexpr const char *kObjc3AdvancedFeatureEvidenceGateScriptPath =
    kObjc3ReleaseEvidenceGateScriptPath;
inline constexpr const char *kObjc3AdvancedFeatureEvidenceGateCommand =
    kObjc3ReleaseEvidenceGatePublicCommand;
inline constexpr const char *kObjc3AdvancedFeatureEvidenceRunbookPath =
    "spec/conformance/release_evidence_gate_maintenance.md";
inline constexpr const char *kObjc3AdvancedFeatureDashboardSchemaPath =
    "schemas/objc3-conformance-dashboard-status-v1.schema.json";
inline constexpr const char *kObjc3ReleaseEvidenceOperationContractId =
    "objc3c.tooling.release.evidence.toolchain.operations.v1";
inline constexpr const char *kObjc3ReleaseEvidenceOperationSchemaId =
    "objc3c-tooling-release-evidence-operation-v1";
inline constexpr const char *kObjc3DashboardStatusPublicationContractId =
    "objc3c.tooling.dashboard.status.publication.v1";
inline constexpr const char *kObjc3DashboardStatusSchemaId =
    "objc3-conformance-dashboard-status/v1";
inline constexpr const char *kObjc3DashboardVersion = "0.11.0";
inline constexpr const char *kObjc3DashboardReleaseId = "objc3c-v0.11";
inline constexpr const char *kObjc3AdvancedFeatureGateContractId =
    "objc3c.tooling.integrated.advanced.feature.gate.v1";
inline constexpr const char *kObjc3AdvancedFeatureGateSchemaId =
    "objc3c-tooling-integrated-advanced-feature-gate-v1";
inline constexpr const char *kObjc3ReleaseCandidateMatrixContractId =
    "objc3c.tooling.release.candidate.execution.matrix.v1";
inline constexpr const char *kObjc3ReleaseCandidateMatrixSchemaId =
    "objc3c-tooling-release-candidate-execution-matrix-v1";
inline constexpr const char *kObjc3AdvancedFeatureReleaseLabel = "v0.11";
inline constexpr const char *kObjc3DeterministicReplayTimestamp =
    "1970-01-01T00:00:00Z";
inline constexpr const char *kObjc3DeterministicSourceRevision = "0000000";
inline constexpr const char *kObjc3ConformanceProfileClaimPolicyModel =
    "core-profile-claimed-strict-profiles-targeted-for-release-evidence-and-fail-closed-until-runtime-backed";
inline constexpr const char *kObjc3ConformanceFormatClaimPolicyModel =
    "json-only-conformance-artifacts-remain-claimable-until-other-formats-gain-validation-and-publication-support";
inline constexpr const char *kObjc3ConformancePublicationFailClosedDiagnosticModel =
    "known-profiles-claimed-json-publication-remains-fail-closed-on-unsupported-formats-and-unknown-profiles";
// scheduler/executor runtime anchor: the driver/process layer still
// does not own scheduling itself, but emitted IR/object evidence now carries a
// frozen private task-runtime helper boundary that later runtime integration
// issues must consume without reconstructing helper or snapshot names ad hoc.
