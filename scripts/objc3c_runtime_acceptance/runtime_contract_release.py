"""Release claim runtime acceptance contract ownership."""

from __future__ import annotations


RUNTIME_CLAIMABLE_SURFACE_RESIDUAL_NON_CLAIMABLE_GAPS_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.claimable.surface.residual.non.claimable.gaps.source.surface.v1"
)
RUNTIME_STRICT_PROFILE_FEATURE_CLAIM_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.strict.profile.feature.claim.source.surface.v1"
)
RUNTIME_CLAIMABILITY_SEMANTICS_RELEASE_POLICY_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.claimability.semantics.release.policy.surface.v1"
)
RUNTIME_STRICT_PROFILE_CLAIM_IMPLEMENTATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.strict.profile.claim.implementation.surface.v1"
)
RUNTIME_RETIRED_ARTIFACT_REJECTION_CONTRACTS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.retired.artifact.rejection.contracts.surface.v1"
)
RUNTIME_CLAIM_PUBLICATION_DASHBOARD_SCHEMA_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.claim.publication.dashboard.schema.surface.v1"
)
RUNTIME_FINAL_CLAIM_PUBLICATION_DEPRECATED_PATH_SHUTDOWN_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.final.claim.publication.deprecated.path.shutdown.surface.v1"
)
RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.release.candidate.claim.abi.surface.v1"
)
RUNTIME_CURRENT_RELEASE_EVIDENCE_OWNER_PAYLOAD_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.current.release.evidence.owner.payload.surface.v1"
)

RELEASE_CLAIMABLE_SURFACE_FIXTURE = "tests/tooling/fixtures/native/hello.objc3"
RETIRED_RELEASE_CLAIM_ARTIFACT_FILENAMES = [
    "module.objc3-release-runtime-claim-matrix.json",
    "module.objc3-dashboard-ready-summary.json",
    "module.objc3-toolchain-runtime-ga-operations-scaffold.json",
]
RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_PROBE = (
    "tests/tooling/runtime/release_candidate_claim_runtime_probe.cpp"
)
RELEASE_CANDIDATE_EVIDENCE_RUNTIME_PROBE = (
    "tests/tooling/runtime/release_candidate_evidence_runtime_probe.cpp"
)


__all__ = [name for name in globals() if name.isupper()]
