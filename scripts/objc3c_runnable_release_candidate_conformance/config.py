"""Constants and path configuration for release-candidate conformance."""

from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
INTEGRATION_REPORT = (
    ROOT / "tmp" / "reports" / "runtime" / "architecture-integration" / "summary.json"
)
ACCEPTANCE_REPORT = ROOT / "tmp" / "reports" / "runtime" / "acceptance" / "summary.json"
REPORT_PATH = (
    ROOT / "tmp" / "reports" / "runtime" / "runnable-release-candidate-conformance" / "summary.json"
)
LIVE_CASE_ROOT = (
    ROOT
    / "tmp"
    / "reports"
    / "runtime"
    / "runnable-release-candidate-conformance"
    / "live-case"
)
RUNNER_PATH = "scripts/check_objc3c_runnable_release_candidate_conformance.py"
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.release.candidate.conformance.summary.v1"

REQUIRED_CASES = {
    "claimable-surface-residual-non-claimable-gaps-source-surface",
    "strict-profile-feature-claim-source-surface",
    "claimability-semantics-release-policy",
    "strict-profile-claim-implementation",
    "retired-artifact-rejection-contracts",
    "claim-publication-dashboard-schema-surface",
    "final-claim-publication-deprecated-path-shutdown",
    "release-candidate-runtime-claim-abi",
    "current-release-evidence-owner-payload",
}

REQUIRED_SURFACE_CONTRACTS = {
    "runtime_claimable_surface_residual_non_claimable_gaps_source_surface": (
        "objc3c.runtime.claimable.surface.residual.non.claimable.gaps.source.surface.v1"
    ),
    "runtime_strict_profile_feature_claim_source_surface": (
        "objc3c.runtime.strict.profile.feature.claim.source.surface.v1"
    ),
    "runtime_claimability_semantics_release_policy_surface": (
        "objc3c.runtime.claimability.semantics.release.policy.surface.v1"
    ),
    "runtime_strict_profile_claim_implementation_surface": (
        "objc3c.runtime.strict.profile.claim.implementation.surface.v1"
    ),
    "runtime_retired_artifact_rejection_contracts_surface": (
        "objc3c.runtime.retired.artifact.rejection.contracts.surface.v1"
    ),
    "runtime_claim_publication_dashboard_schema_surface": (
        "objc3c.runtime.claim.publication.dashboard.schema.surface.v1"
    ),
    "runtime_final_claim_publication_deprecated_path_shutdown_surface": (
        "objc3c.runtime.final.claim.publication.deprecated.path.shutdown.surface.v1"
    ),
    "runtime_release_candidate_claim_abi_surface": (
        "objc3c.runtime.release.candidate.claim.abi.surface.v1"
    ),
    "runtime_current_release_evidence_owner_payload_surface": (
        "objc3c.runtime.current.release.evidence.owner.payload.surface.v1"
    ),
}

TARGETED_PROFILE_IDS = ["strict", "strict-concurrency", "strict-system"]
DASHBOARD_SCHEMA_PATH = "schemas/objc3-conformance-dashboard-status-v1.schema.json"
RETIRED_RELEASE_CLAIM_ARTIFACT_FILENAMES = [
    "module.objc3-release-runtime-claim-matrix.json",
    "module.objc3-dashboard-ready-summary.json",
    "module.objc3-toolchain-runtime-ga-operations-scaffold.json",
]


__all__ = [
    "ACCEPTANCE_REPORT",
    "DASHBOARD_SCHEMA_PATH",
    "RETIRED_RELEASE_CLAIM_ARTIFACT_FILENAMES",
    "INTEGRATION_REPORT",
    "LIVE_CASE_ROOT",
    "REPORT_PATH",
    "REQUIRED_CASES",
    "REQUIRED_SURFACE_CONTRACTS",
    "ROOT",
    "RUNNER_PATH",
    "SCRIPTS_ROOT",
    "SUMMARY_CONTRACT_ID",
    "TARGETED_PROFILE_IDS",
]
