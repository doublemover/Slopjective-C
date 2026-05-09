"""Release-claim runtime acceptance owner contracts."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


RELEASE_CLAIMS_OWNER_CONTRACT_ID = (
    "objc3c.runtime.release.claims.owner.contract.v1"
)
RELEASE_CLAIMS_STRICT_STATUS_CONTRACT_ID = (
    "objc3c.runtime.release.claims.strict.status.owner.v1"
)

CLAIMABLE_SURFACE_OWNER = "release-claims.claimable-surface"
RELEASE_POLICY_OWNER = "release-claims.release-policy"
RETIRED_ARTIFACT_REJECTION_OWNER = "release-claims.retired-artifact-rejection"
DASHBOARD_PUBLICATION_OWNER = "release-claims.dashboard-publication"
FINAL_PUBLICATION_OWNER = "release-claims.final-publication"
RUNTIME_CLAIM_ABI_OWNER = "release-claims.runtime-claim-abi"
CURRENT_RELEASE_EVIDENCE_OWNER = "release-claims.current-release-evidence"
STRICT_STATUS_OWNER = "release-claims.strict-status"

RELEASE_CLAIMS_OWNER_CASE_IDS: tuple[str, ...] = (
    "claimable-surface-residual-non-claimable-gaps-source-surface",
    "strict-profile-feature-claim-source-surface",
    "claimability-semantics-release-policy",
    "strict-profile-claim-implementation",
    "retired-artifact-rejection-contracts",
    "claim-publication-dashboard-schema-surface",
    "final-claim-publication-deprecated-path-shutdown",
    "release-candidate-runtime-claim-abi",
    "current-release-evidence-owner-payload",
)

RELEASE_CLAIMS_REQUIRED_OWNER_IDS: tuple[str, ...] = (
    CLAIMABLE_SURFACE_OWNER,
    RELEASE_POLICY_OWNER,
    RETIRED_ARTIFACT_REJECTION_OWNER,
    DASHBOARD_PUBLICATION_OWNER,
    FINAL_PUBLICATION_OWNER,
    RUNTIME_CLAIM_ABI_OWNER,
    CURRENT_RELEASE_EVIDENCE_OWNER,
    STRICT_STATUS_OWNER,
)


@dataclass(frozen=True)
class ReleaseClaimsOwnerContract:
    owner_id: str
    owner_surface: str
    case_ids: tuple[str, ...]
    source_modules: tuple[str, ...]
    owned_decisions: tuple[str, ...]

    def payload(self) -> dict[str, Any]:
        return {
            "contract_id": RELEASE_CLAIMS_OWNER_CONTRACT_ID,
            "owner_id": self.owner_id,
            "owner_surface": self.owner_surface,
            "case_ids": list(self.case_ids),
            "source_modules": list(self.source_modules),
            "owned_decisions": list(self.owned_decisions),
        }


CLAIMABLE_SURFACE_OWNER_CONTRACT = ReleaseClaimsOwnerContract(
    owner_id=CLAIMABLE_SURFACE_OWNER,
    owner_surface="claimable profile source surfaces and strict feature truth",
    case_ids=(
        "claimable-surface-residual-non-claimable-gaps-source-surface",
        "strict-profile-feature-claim-source-surface",
    ),
    source_modules=(
        "release_claims_claimable_artifacts",
        "release_claims_claimable_cases",
        "release_claims_claimable_residual_assertions",
        "release_claims_claimable_strict_profile_assertions",
        "release_claims_source_surfaces",
    ),
    owned_decisions=(
        "claimable profile inventory",
        "strict profile feature truth boundaries",
        "compile-coupled release gate artifact expectations",
    ),
)

RELEASE_POLICY_OWNER_CONTRACT = ReleaseClaimsOwnerContract(
    owner_id=RELEASE_POLICY_OWNER,
    owner_surface="release claimability policy and strict profile implementation",
    case_ids=(
        "claimability-semantics-release-policy",
        "strict-profile-claim-implementation",
    ),
    source_modules=(
        "release_claims_policy_cases",
        "release_claims_policy_claimability_case",
        "release_claims_policy_strict_profile_case",
        "release_claims_source_surfaces",
    ),
    owned_decisions=(
        "release profile format policy",
        "strict profile validation support",
        "single native CLI policy source of truth",
    ),
)

RETIRED_ARTIFACT_REJECTION_OWNER_CONTRACT = ReleaseClaimsOwnerContract(
    owner_id=RETIRED_ARTIFACT_REJECTION_OWNER,
    owner_surface="retired release artifact rejection contracts",
    case_ids=("retired-artifact-rejection-contracts",),
    source_modules=(
        "release_claims_publication_diagnostic_cases",
        "release_claims_publication_surfaces",
        "release_claims_retired_artifacts",
    ),
    owned_decisions=(
        "compile-time rejection of retired release artifacts",
        "validation-time rejection of retired release artifacts",
        "retired artifact filename inventory",
    ),
)

DASHBOARD_PUBLICATION_OWNER_CONTRACT = ReleaseClaimsOwnerContract(
    owner_id=DASHBOARD_PUBLICATION_OWNER,
    owner_surface="current release dashboard publication payloads",
    case_ids=("claim-publication-dashboard-schema-surface",),
    source_modules=(
        "release_claims_publication_dashboard_assertions",
        "release_claims_publication_dashboard_cases",
        "release_claims_publication_surfaces",
    ),
    owned_decisions=(
        "dashboard schema identity",
        "current release artifact references",
        "dashboard refresh telemetry",
    ),
)

FINAL_PUBLICATION_OWNER_CONTRACT = ReleaseClaimsOwnerContract(
    owner_id=FINAL_PUBLICATION_OWNER,
    owner_surface="final current release publication artifact set",
    case_ids=("final-claim-publication-deprecated-path-shutdown",),
    source_modules=(
        "release_claims_publication_shutdown_assertions",
        "release_claims_publication_shutdown_cases",
        "release_claims_publication_surfaces",
    ),
    owned_decisions=(
        "current compile artifact inventory",
        "current validation artifact inventory",
        "retired artifact absence in final publication paths",
    ),
)

RUNTIME_CLAIM_ABI_OWNER_CONTRACT = ReleaseClaimsOwnerContract(
    owner_id=RUNTIME_CLAIM_ABI_OWNER,
    owner_surface="release candidate claim runtime ABI evidence",
    case_ids=("release-candidate-runtime-claim-abi",),
    source_modules=(
        "release_claims_runtime_abi_cases",
        "release_claims_runtime_cases",
        "release_claims_runtime_surfaces",
    ),
    owned_decisions=(
        "private claim snapshot ABI",
        "runtime claim profile payload",
        "live publication contract set observed by the probe",
    ),
)

CURRENT_RELEASE_EVIDENCE_OWNER_CONTRACT = ReleaseClaimsOwnerContract(
    owner_id=CURRENT_RELEASE_EVIDENCE_OWNER,
    owner_surface="current release evidence owner payloads",
    case_ids=("current-release-evidence-owner-payload",),
    source_modules=(
        "release_claims_runtime_evidence_assertions",
        "release_claims_runtime_evidence_cases",
        "release_claims_runtime_surfaces",
    ),
    owned_decisions=(
        "current release evidence artifact inventory",
        "private release evidence runtime probe payload",
        "retired artifact rejection state carried into runtime evidence",
    ),
)

RELEASE_CLAIMS_OWNER_CONTRACTS: tuple[ReleaseClaimsOwnerContract, ...] = (
    CLAIMABLE_SURFACE_OWNER_CONTRACT,
    RELEASE_POLICY_OWNER_CONTRACT,
    RETIRED_ARTIFACT_REJECTION_OWNER_CONTRACT,
    DASHBOARD_PUBLICATION_OWNER_CONTRACT,
    FINAL_PUBLICATION_OWNER_CONTRACT,
    RUNTIME_CLAIM_ABI_OWNER_CONTRACT,
    CURRENT_RELEASE_EVIDENCE_OWNER_CONTRACT,
)

_CASE_OWNER_CONTRACTS = {
    case_id: contract
    for contract in RELEASE_CLAIMS_OWNER_CONTRACTS
    for case_id in contract.case_ids
}


def release_claims_owner_contract_payloads() -> list[dict[str, Any]]:
    return [contract.payload() for contract in RELEASE_CLAIMS_OWNER_CONTRACTS]


def release_claims_strict_status_owner_payload() -> dict[str, Any]:
    return {
        "contract_id": RELEASE_CLAIMS_STRICT_STATUS_CONTRACT_ID,
        "owner_id": STRICT_STATUS_OWNER,
        "owner_surface": "release-claims runtime acceptance status",
        "case_ids": list(RELEASE_CLAIMS_OWNER_CASE_IDS),
        "status_policy": (
            "release-claim acceptance status is derived from current CaseResult "
            "payloads and owner coverage raises on unmapped cases"
        ),
    }


def release_claims_case_owner_payload(case_id: str) -> dict[str, Any]:
    contract = _CASE_OWNER_CONTRACTS.get(case_id)
    if contract is None:
        raise ValueError(f"unowned release-claims runtime acceptance case: {case_id}")
    payload = contract.payload()
    payload["strict_status_owner"] = release_claims_strict_status_owner_payload()
    return payload


def release_claims_case_summary(
    case_id: str,
    summary: dict[str, Any],
) -> dict[str, Any]:
    return {
        "owner_contract": release_claims_case_owner_payload(case_id),
        **summary,
    }


def release_claims_surface_owner_payload() -> dict[str, Any]:
    return {
        "contract_id": RELEASE_CLAIMS_OWNER_CONTRACT_ID,
        "owner_ids": list(RELEASE_CLAIMS_REQUIRED_OWNER_IDS),
        "owner_contracts": release_claims_owner_contract_payloads(),
        "strict_status_owner": release_claims_strict_status_owner_payload(),
    }


__all__ = [
    "CLAIMABLE_SURFACE_OWNER",
    "CLAIMABLE_SURFACE_OWNER_CONTRACT",
    "CURRENT_RELEASE_EVIDENCE_OWNER",
    "CURRENT_RELEASE_EVIDENCE_OWNER_CONTRACT",
    "DASHBOARD_PUBLICATION_OWNER",
    "DASHBOARD_PUBLICATION_OWNER_CONTRACT",
    "FINAL_PUBLICATION_OWNER",
    "FINAL_PUBLICATION_OWNER_CONTRACT",
    "RELEASE_CLAIMS_OWNER_CASE_IDS",
    "RELEASE_CLAIMS_OWNER_CONTRACTS",
    "RELEASE_CLAIMS_OWNER_CONTRACT_ID",
    "RELEASE_CLAIMS_REQUIRED_OWNER_IDS",
    "RELEASE_CLAIMS_STRICT_STATUS_CONTRACT_ID",
    "RELEASE_POLICY_OWNER",
    "RELEASE_POLICY_OWNER_CONTRACT",
    "RETIRED_ARTIFACT_REJECTION_OWNER",
    "RETIRED_ARTIFACT_REJECTION_OWNER_CONTRACT",
    "RUNTIME_CLAIM_ABI_OWNER",
    "RUNTIME_CLAIM_ABI_OWNER_CONTRACT",
    "STRICT_STATUS_OWNER",
    "ReleaseClaimsOwnerContract",
    "release_claims_case_owner_payload",
    "release_claims_case_summary",
    "release_claims_owner_contract_payloads",
    "release_claims_strict_status_owner_payload",
    "release_claims_surface_owner_payload",
]
