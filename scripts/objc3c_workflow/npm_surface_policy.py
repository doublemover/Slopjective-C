"""Owned package-script surface policy for workflow bridge metadata."""

from __future__ import annotations

from collections.abc import Mapping
from dataclasses import asdict, dataclass

from .public_bridge_constants import (
    PUBLIC_BRIDGE_INTEGRITY_CONTRACT_ID,
    WORKFLOW_BRIDGE_SCRIPT,
    WORKFLOW_PUBLIC_COMMAND_TEMPLATE,
)
from .public_bridge_integrity import public_bridge_integrity
from .public_bridge_model import PackageBridgeSpec

NPM_SURFACE_POLICY_OWNER = "objc3c-workflow-npm-surface-policy"
PACKAGE_SCRIPT_LOOKUP_OWNER = "objc3c-workflow-package-script-lookup"
PACKAGE_SCRIPT_LOOKUP_CONTRACT_ID = "objc3c-workflow-package-script-lookup-v1"


@dataclass(frozen=True)
class PackageScriptLookupDecision:
    contract_id: str
    owner_surface: str
    script_name: str
    registered: bool
    canonical_script: str
    canonical_invocation_template: str
    public_bridge_integrity_contract: str
    registered_script_count: int
    retired_metadata_allowed: bool
    validation_errors: tuple[str, ...]


def package_script_lookup_decision(
    package_bridges: Mapping[str, PackageBridgeSpec],
    script_name: str,
) -> PackageScriptLookupDecision:
    integrity = public_bridge_integrity(package_bridges)
    errors = list(integrity.validation_errors)
    registered = integrity.valid and script_name == WORKFLOW_BRIDGE_SCRIPT
    if script_name != WORKFLOW_BRIDGE_SCRIPT:
        errors.append(f"unknown package script: {script_name}")

    return PackageScriptLookupDecision(
        contract_id=PACKAGE_SCRIPT_LOOKUP_CONTRACT_ID,
        owner_surface=PACKAGE_SCRIPT_LOOKUP_OWNER,
        script_name=script_name,
        registered=registered,
        canonical_script=WORKFLOW_BRIDGE_SCRIPT,
        canonical_invocation_template=WORKFLOW_PUBLIC_COMMAND_TEMPLATE,
        public_bridge_integrity_contract=PUBLIC_BRIDGE_INTEGRITY_CONTRACT_ID,
        registered_script_count=len(package_bridges),
        retired_metadata_allowed=False,
        validation_errors=tuple(errors),
    )


def package_script_lookup_fields(
    package_bridges: Mapping[str, PackageBridgeSpec],
    script_name: str,
) -> dict[str, object]:
    return asdict(package_script_lookup_decision(package_bridges, script_name))


def package_script_is_registered(
    package_bridges: Mapping[str, PackageBridgeSpec],
    script_name: str,
) -> bool:
    return package_script_lookup_decision(package_bridges, script_name).registered


def package_script_payload_name(script_name: str) -> str:
    if script_name != WORKFLOW_BRIDGE_SCRIPT:
        raise ValueError(f"unknown package script: {script_name}")
    return script_name


__all__ = [
    "NPM_SURFACE_POLICY_OWNER",
    "PACKAGE_SCRIPT_LOOKUP_CONTRACT_ID",
    "PACKAGE_SCRIPT_LOOKUP_OWNER",
    "PackageScriptLookupDecision",
    "package_script_lookup_decision",
    "package_script_lookup_fields",
    "package_script_is_registered",
    "package_script_payload_name",
]
