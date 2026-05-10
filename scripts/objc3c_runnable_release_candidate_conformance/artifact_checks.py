"""Artifact and surface checks for release-candidate conformance."""

from __future__ import annotations

from typing import Any

from .config import (
    ACCEPTANCE_REPORT,
    DASHBOARD_SCHEMA_PATH,
    DEPRECATED_SIDECAR_FILENAMES,
    INTEGRATION_REPORT,
    REQUIRED_CASES,
    REQUIRED_SURFACE_CONTRACTS,
    TARGETED_PROFILE_IDS,
)
from .io import repo_rel


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def build_report_case_map(acceptance_report: Any) -> dict[str, dict[str, Any]]:
    if not isinstance(acceptance_report, dict):
        return {}

    cases = acceptance_report.get("cases", [])
    if not isinstance(cases, list):
        return {}

    return {
        str(case.get("case_id")): case
        for case in cases
        if isinstance(case, dict) and case.get("case_id") is not None
    }


def report_has_required_cases(case_map: dict[str, dict[str, Any]]) -> bool:
    return bool(case_map) and all(
        case_map.get(case_id, {}).get("passed") is True for case_id in REQUIRED_CASES
    )


def report_has_required_surfaces(acceptance_report: Any) -> bool:
    return isinstance(acceptance_report, dict) and all(
        isinstance(acceptance_report.get(surface_key), dict)
        for surface_key in REQUIRED_SURFACE_CONTRACTS
    )


def validate_live_results(results: list[Any]) -> None:
    for result in results:
        expect(
            result.passed is True,
            f"required release-candidate case {result.case_id} did not pass",
        )
    expect(
        {result.case_id for result in results} == REQUIRED_CASES,
        "live release-candidate case collection drifted from the required case set",
    )


def validate_surface_contracts(surfaces: dict[str, dict[str, Any]]) -> None:
    for surface_key, contract_id in REQUIRED_SURFACE_CONTRACTS.items():
        surface = surfaces.get(surface_key)
        expect(
            isinstance(surface, dict),
            f"release-candidate conformance did not publish {surface_key}",
        )
        expect(
            surface.get("contract_id") == contract_id,
            f"release-candidate conformance published the wrong contract id for {surface_key}",
        )


def compare_integration_surfaces(
    integration_report: Any,
    surfaces: dict[str, dict[str, Any]],
) -> str:
    if not (
        isinstance(integration_report, dict)
        and integration_report.get("status") == "PASS"
    ):
        return "not-available"

    integration_surfaces_present = all(
        isinstance(integration_report.get(surface_key), dict)
        for surface_key in REQUIRED_SURFACE_CONTRACTS
    )
    if not integration_surfaces_present:
        return "stale-or-missing"

    for surface_key, surface in surfaces.items():
        expect(
            integration_report.get(surface_key) == surface,
            f"runtime integration report drifted from live release-candidate surface {surface_key}",
        )
    return "matched"


def validate_release_candidate_surface_relationships(
    surfaces: dict[str, dict[str, Any]],
) -> None:
    claim_surface = surfaces["runtime_release_candidate_claim_abi_surface"]
    evidence_surface = surfaces[
        "runtime_final_release_evidence_descaffolding_implementation_surface"
    ]
    strict_claim_surface = surfaces[
        "runtime_strict_profile_claim_implementation_surface"
    ]
    final_publication_surface = surfaces[
        "runtime_final_claim_publication_deprecated_path_shutdown_surface"
    ]
    dashboard_surface = surfaces[
        "runtime_claim_publication_dashboard_schema_surface"
    ]

    expect(
        claim_surface.get("claimed_profile_ids")
        == strict_claim_surface.get("claimed_profile_ids"),
        "release-candidate claim ABI surface drifted from the strict-profile claim implementation profile set",
    )
    expect(
        claim_surface.get("targeted_profile_ids") == TARGETED_PROFILE_IDS,
        "release-candidate claim ABI surface drifted from the targeted strict profile set",
    )
    expect(
        evidence_surface.get("runtime_release_candidate_claim_abi_surface_contract_id")
        == claim_surface.get("contract_id"),
        "final release evidence implementation surface drifted from the release-candidate claim ABI contract",
    )
    expect(
        dashboard_surface.get("dashboard_schema_path") == DASHBOARD_SCHEMA_PATH,
        "claim dashboard schema surface drifted from the live dashboard schema path",
    )
    expect(
        final_publication_surface.get("deprecated_sidecar_filenames")
        == DEPRECATED_SIDECAR_FILENAMES,
        "final claim publication surface drifted from the retired sidecar inventory",
    )


def build_child_report_paths(
    acceptance_report: Any,
    integration_report: Any,
) -> list[str]:
    child_report_paths = []
    if isinstance(acceptance_report, dict):
        child_report_paths.append(repo_rel(ACCEPTANCE_REPORT))
    if isinstance(integration_report, dict):
        child_report_paths.append(repo_rel(INTEGRATION_REPORT))
    return child_report_paths


__all__ = [
    "build_child_report_paths",
    "build_report_case_map",
    "compare_integration_surfaces",
    "expect",
    "report_has_required_cases",
    "report_has_required_surfaces",
    "validate_live_results",
    "validate_release_candidate_surface_relationships",
    "validate_surface_contracts",
]
