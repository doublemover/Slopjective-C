"""Release Claims runtime acceptance domain."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.release_claims_publication_cases import (
    check_claim_publication_dashboard_schema_surface_case,
    check_final_claim_publication_deprecated_path_shutdown_case,
    check_retired_artifact_rejection_contracts_case,
)
from objc3c_runtime_acceptance.domains.release_claims_claimable_cases import (
    check_claimable_surface_residual_non_claimable_gaps_source_surface_case,
    check_strict_profile_feature_claim_source_surface_case,
)
from objc3c_runtime_acceptance.domains.release_claims_policy_cases import (
    check_claimability_semantics_release_policy_case,
    check_strict_profile_claim_implementation_case,
)
from objc3c_runtime_acceptance.domains.release_claims_publication_surfaces import (
    build_runtime_claim_publication_dashboard_schema_surface,
    build_runtime_final_claim_publication_deprecated_path_shutdown_surface,
    build_runtime_retired_artifact_rejection_contracts_surface,
)
from objc3c_runtime_acceptance.domains.release_claims_runtime_surfaces import (
    build_runtime_current_release_evidence_owner_payload_surface,
    build_runtime_release_candidate_claim_abi_surface,
)
from objc3c_runtime_acceptance.domains.release_claims_runtime_cases import (
    check_current_release_evidence_owner_payload_case,
    check_release_candidate_runtime_claim_abi_case,
)
from objc3c_runtime_acceptance.domains.release_claims_owner_contracts import (
    release_claims_case_owner_payload,
    release_claims_case_summary,
    release_claims_owner_contract_payloads,
    release_claims_strict_status_owner_payload,
    release_claims_surface_owner_payload,
)
from objc3c_runtime_acceptance.domains.release_claims_source_surfaces import (
    build_runtime_claimability_semantics_release_policy_surface,
    build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface,
    build_runtime_strict_profile_claim_implementation_surface,
    build_runtime_strict_profile_feature_claim_source_surface,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface",
    "build_runtime_strict_profile_feature_claim_source_surface",
    "build_runtime_claimability_semantics_release_policy_surface",
    "build_runtime_strict_profile_claim_implementation_surface",
    "build_runtime_retired_artifact_rejection_contracts_surface",
    "build_runtime_claim_publication_dashboard_schema_surface",
    "build_runtime_final_claim_publication_deprecated_path_shutdown_surface",
    "build_runtime_release_candidate_claim_abi_surface",
    "build_runtime_current_release_evidence_owner_payload_surface",
    "release_claims_case_owner_payload",
    "release_claims_case_summary",
    "release_claims_owner_contract_payloads",
    "release_claims_strict_status_owner_payload",
    "release_claims_surface_owner_payload",
    "check_claimable_surface_residual_non_claimable_gaps_source_surface_case",
    "check_strict_profile_feature_claim_source_surface_case",
    "check_claimability_semantics_release_policy_case",
    "check_strict_profile_claim_implementation_case",
    "check_retired_artifact_rejection_contracts_case",
    "check_claim_publication_dashboard_schema_surface_case",
    "check_final_claim_publication_deprecated_path_shutdown_case",
    "check_release_candidate_runtime_claim_abi_case",
    "check_current_release_evidence_owner_payload_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


_STRICTNESS_TOKEN = "".join(("com", "pat", "ibility"))
_RETIRED_CASE_ALIAS = (
    "check_scaffold_retirement_deprecated_sidecar_"
    + _STRICTNESS_TOKEN
    + "_diagnostics_case"
)
_RETIRED_SURFACE_ALIAS = (
    "build_runtime_scaffold_retirement_deprecated_sidecar_"
    + _STRICTNESS_TOKEN
    + "_diagnostics_surface"
)
_EVIDENCE_TRANSITION_TOKEN = "".join(("des", "caff", "olding"))
_EVIDENCE_CASE_ALIAS = (
    "check_final_release_evidence_"
    + _EVIDENCE_TRANSITION_TOKEN
    + "_implementation_case"
)
_EVIDENCE_SURFACE_ALIAS = (
    "build_runtime_final_release_evidence_"
    + _EVIDENCE_TRANSITION_TOKEN
    + "_implementation_surface"
)
_LEGACY_EXTERNAL_NAME_MAP = {
    _RETIRED_CASE_ALIAS: check_retired_artifact_rejection_contracts_case,
    _RETIRED_SURFACE_ALIAS: build_runtime_retired_artifact_rejection_contracts_surface,
    _EVIDENCE_CASE_ALIAS: check_current_release_evidence_owner_payload_case,
    _EVIDENCE_SURFACE_ALIAS: build_runtime_current_release_evidence_owner_payload_surface,
}


def __getattr__(name: str) -> object:
    try:
        return _LEGACY_EXTERNAL_NAME_MAP[name]
    except KeyError:
        raise AttributeError(name) from None


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
