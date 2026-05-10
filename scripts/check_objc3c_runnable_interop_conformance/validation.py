"""Conformance validation for runnable interop surfaces."""

from __future__ import annotations

from typing import Any

from .assertions import expect
from .constants import REQUIRED_CASES, REQUIRED_SURFACE_CONTRACTS
from .live_cases import runtime_acceptance


def validate_live_results(results: list[runtime_acceptance.CaseResult]) -> None:
    for result in results:
        expect(result.passed is True, f"required interop case {result.case_id} did not pass")
    expect(
        {result.case_id for result in results} == REQUIRED_CASES,
        "live interop case collection drifted from the required case set",
    )


def validate_required_surface_contracts(surfaces: dict[str, dict[str, Any]]) -> None:
    for surface_key, contract_id in REQUIRED_SURFACE_CONTRACTS.items():
        surface = surfaces.get(surface_key)
        expect(
            isinstance(surface, dict),
            f"interop conformance did not publish {surface_key}",
        )
        expect(
            surface.get("contract_id") == contract_id,
            f"interop conformance published the wrong contract id for {surface_key}",
        )


def integration_surface_comparison(
    integration_report: dict[str, Any] | None,
    surfaces: dict[str, dict[str, Any]],
) -> str:
    if not isinstance(integration_report, dict) or integration_report.get("status") != "PASS":
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
            f"runtime integration report drifted from live interop surface {surface_key}",
        )
    return "matched"


def validate_runtime_package_loading_surfaces(
    surfaces: dict[str, dict[str, Any]]
) -> tuple[dict[str, Any], dict[str, Any], dict[str, Any]]:
    package_loading_surface = surfaces[
        "runtime_package_loading_module_identity_semantics_surface"
    ]
    abi_surface = surfaces["runtime_package_loader_bridge_abi_surface"]
    implementation_surface = surfaces[
        "runtime_package_loading_interop_implementation_surface"
    ]
    expect(
        "imported-runtime-packaging-replay"
        in package_loading_surface.get("authoritative_case_ids", []),
        "package-loading module identity surface did not carry the imported-runtime-packaging-replay case",
    )
    expect(
        "runtime-package-loader-bridge-abi" in abi_surface.get("authoritative_case_ids", []),
        "package-loader bridge ABI surface did not carry the runtime-package-loader-bridge-abi case",
    )
    expect(
        "live-package-loading-interop-runtime-implementation"
        in implementation_surface.get("authoritative_case_ids", []),
        "package-loading interop implementation surface did not carry the live implementation case",
    )
    expect(
        implementation_surface.get("authoritative_probe_paths")
        == abi_surface.get("authoritative_probe_paths"),
        "interop implementation surface drifted from the runtime package-loader ABI probe set",
    )
    return package_loading_surface, abi_surface, implementation_surface


__all__ = [
    "integration_surface_comparison",
    "validate_live_results",
    "validate_required_surface_contracts",
    "validate_runtime_package_loading_surfaces",
]
