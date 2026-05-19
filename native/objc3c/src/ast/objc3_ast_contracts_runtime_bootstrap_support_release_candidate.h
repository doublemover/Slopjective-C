#pragma once

inline constexpr const char *kObjc3RuntimeInstallationAbiSurfaceContractId =
    "objc3c.runtime.installation.abi.surface.v1";
inline constexpr const char *kObjc3RuntimeLoaderLifecycleSurfaceContractId =
    "objc3c.runtime.loader.lifecycle.surface.v1";
inline constexpr const char *kObjc3RuntimeReleaseCandidateClaimAbiSurfaceContractId =
    "objc3c.runtime.release.candidate.claim.abi.surface.v1";
inline constexpr const char *kObjc3RuntimeReleaseCandidateClaimSnapshotSymbol =
    "objc3_runtime_copy_release_candidate_claim_snapshot_for_testing";
inline constexpr const char *kObjc3RuntimeReleaseCandidateClaimSnapshotType =
    "objc3_runtime_release_candidate_claim_snapshot";
inline constexpr const char *kObjc3RuntimeReleaseCandidateClaimProbePath =
    "tests/tooling/runtime/release_candidate_claim_runtime_probe.cpp";
inline constexpr const char *kObjc3RuntimeReleaseCandidateClaimBoundaryModel =
    "private-release-candidate-claim-snapshot-freezes-the-final-claim-publication-contract-set-and-retired-artifact-rejection-without-widening-the-public-runtime-header";
inline constexpr const char
    *kObjc3RuntimeCurrentReleaseEvidenceOwnerPayloadSurfaceContractId =
        "objc3c.runtime.current.release.evidence.owner.payload.surface.v1";
inline constexpr const char *kObjc3RuntimeReleaseCandidateEvidenceSnapshotSymbol =
    "objc3_runtime_copy_release_candidate_evidence_state_for_testing";
inline constexpr const char *kObjc3RuntimeReleaseCandidateEvidenceSnapshotType =
    "objc3_runtime_release_candidate_evidence_state_snapshot";
inline constexpr const char *kObjc3RuntimeReleaseCandidateEvidenceProbePath =
    "tests/tooling/runtime/release_candidate_evidence_runtime_probe.cpp";
inline constexpr const char *kObjc3RuntimeCurrentReleaseEvidenceOwnerPayloadModel =
    "private-release-candidate-evidence-snapshot-freezes-the-live-validation-release-evidence-dashboard-gate-matrix-and-retired-artifact-rejection-boundary";
