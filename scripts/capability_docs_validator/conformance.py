from __future__ import annotations

from typing import Any

from objc3c_tooling.paths import resolve_repo_path

from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.manifest import (
    _manifest_fixture_paths,
    _manifest_support_claims,
)


def _phase_contract_rows(phase_owner_contracts: dict[str, Any]) -> list[dict[str, Any]]:
    raw_contracts = phase_owner_contracts.get("phase_contracts")
    if not isinstance(raw_contracts, list) or not raw_contracts:
        raise CapabilityDocsError("phase owner contract manifest must have phase_contracts")
    contracts: list[dict[str, Any]] = []
    seen_phases: set[str] = set()
    for index, contract in enumerate(raw_contracts):
        if not isinstance(contract, dict):
            raise CapabilityDocsError(f"phase_contracts[{index}] must be an object")
        phase = contract.get("phase")
        if not isinstance(phase, str) or not phase:
            raise CapabilityDocsError(f"phase_contracts[{index}].phase must be non-empty")
        if phase in seen_phases:
            raise CapabilityDocsError(f"duplicate phase owner contract: {phase}")
        seen_phases.add(phase)
        contracts.append(contract)
    return contracts


def _validate_fixture_list(
    *,
    phase: str,
    label: str,
    raw_paths: Any,
    fixture_paths: set[str],
) -> None:
    if not isinstance(raw_paths, list) or not raw_paths:
        raise CapabilityDocsError(f"{phase} {label} must be a non-empty list")
    for index, raw_path in enumerate(raw_paths):
        if not isinstance(raw_path, str) or not raw_path:
            raise CapabilityDocsError(f"{phase} {label}[{index}] must be non-empty")
        if raw_path not in fixture_paths:
            raise CapabilityDocsError(
                f"{phase} {label}[{index}] is not listed in canonical manifest fixtures: "
                f"{raw_path}"
            )
        if not resolve_repo_path(raw_path).is_file():
            raise CapabilityDocsError(f"{phase} {label}[{index}] fixture is missing: {raw_path}")


def _validate_conformance_manifest_links(
    manifest: dict[str, Any],
    phase_owner_contracts: dict[str, Any],
) -> None:
    manifest_claims = _manifest_support_claims(manifest)
    fixture_paths = _manifest_fixture_paths(manifest)

    for contract in _phase_contract_rows(phase_owner_contracts):
        phase = str(contract["phase"])
        support_claim = contract.get("support_claim")
        if not isinstance(support_claim, str) or not support_claim:
            raise CapabilityDocsError(f"{phase} support_claim must be non-empty")
        if support_claim not in manifest_claims:
            raise CapabilityDocsError(
                f"{phase} support_claim is not backed by canonical manifest: "
                f"{support_claim}"
            )
        owner_phase = manifest_claims[support_claim]["owner_phase"]
        if owner_phase != phase:
            raise CapabilityDocsError(
                f"{support_claim} owner phase mismatch: manifest={owner_phase}, "
                f"phase_contract={phase}"
            )
        if contract.get("generated_fixture_authority") is not False:
            raise CapabilityDocsError(
                f"{phase} generated_fixture_authority must be false for support claims"
            )

        _validate_fixture_list(
            phase=phase,
            label="canonical_positive_evidence",
            raw_paths=contract.get("canonical_positive_evidence"),
            fixture_paths=fixture_paths,
        )
        _validate_fixture_list(
            phase=phase,
            label="retired_surface_evidence",
            raw_paths=contract.get("retired_surface_evidence"),
            fixture_paths=fixture_paths,
        )
