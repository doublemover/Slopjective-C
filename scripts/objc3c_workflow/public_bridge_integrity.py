"""Fail-closed integrity checks for the single public workflow bridge."""

from __future__ import annotations

from collections.abc import Mapping
from dataclasses import asdict, dataclass

from .public_bridge_constants import (
    PUBLIC_BRIDGE_CAPABILITY_TRUTH_SCOPE,
    PUBLIC_BRIDGE_CONSTANTS_OWNER_SURFACE,
    PUBLIC_BRIDGE_INTEGRITY_CONTRACT_ID,
    PUBLIC_BRIDGE_INTEGRITY_OWNER_SURFACE,
    PUBLIC_BRIDGE_INVOCATION_OWNER_SURFACE,
    PUBLIC_BRIDGE_PAYLOAD_OWNER_SURFACE,
    PUBLIC_BRIDGE_REGISTRY_OWNER_SURFACE,
    PUBLIC_ENTRYPOINT_KIND,
    WORKFLOW_BRIDGE_SCRIPT,
    WORKFLOW_PUBLIC_ACTION_TOKEN,
    WORKFLOW_PUBLIC_COMMAND_TEMPLATE,
    WORKFLOW_RUNNER_MODE,
    WORKFLOW_RUNNER_SURFACE,
)
from .public_bridge_model import PackageBridgeSpec


@dataclass(frozen=True)
class PublicBridgeIntegrity:
    contract_id: str
    owner_surface: str
    package_bridge_count: int
    canonical_package_bridge: str
    canonical_invocation_template: str
    single_package_bridge_only: bool
    retired_metadata_allowed: bool
    pass_through_args_required: bool
    validation_errors: tuple[str, ...]

    @property
    def valid(self) -> bool:
        return not self.validation_errors


def _validate_bridge_spec(key: str, spec: PackageBridgeSpec) -> list[str]:
    fields = asdict(spec)
    errors: list[str] = []
    if key != WORKFLOW_BRIDGE_SCRIPT:
        errors.append(f"unexpected package bridge key: {key}")
    if spec.package_bridge != WORKFLOW_BRIDGE_SCRIPT:
        errors.append(f"unexpected package bridge name: {spec.package_bridge}")
    if spec.action != WORKFLOW_PUBLIC_ACTION_TOKEN:
        errors.append(f"unexpected public action token: {spec.action}")
    if spec.backend != WORKFLOW_PUBLIC_COMMAND_TEMPLATE:
        errors.append(f"unexpected bridge backend: {spec.backend}")
    if spec.invocation_template != WORKFLOW_PUBLIC_COMMAND_TEMPLATE:
        errors.append(f"unexpected invocation template: {spec.invocation_template}")
    if spec.mode != WORKFLOW_RUNNER_MODE:
        errors.append(f"unexpected runner mode: {spec.mode}")
    if spec.runner_path != WORKFLOW_RUNNER_SURFACE:
        errors.append(f"unexpected runner path: {spec.runner_path}")
    if spec.public_entrypoint != PUBLIC_ENTRYPOINT_KIND:
        errors.append(f"unexpected public entrypoint: {spec.public_entrypoint}")
    if spec.constants_owner_surface != PUBLIC_BRIDGE_CONSTANTS_OWNER_SURFACE:
        errors.append("constants owner surface drifted")
    if spec.invocation_owner_surface != PUBLIC_BRIDGE_INVOCATION_OWNER_SURFACE:
        errors.append("invocation owner surface drifted")
    if spec.payload_owner_surface != PUBLIC_BRIDGE_PAYLOAD_OWNER_SURFACE:
        errors.append("payload owner surface drifted")
    if spec.registry_owner_surface != PUBLIC_BRIDGE_REGISTRY_OWNER_SURFACE:
        errors.append("registry owner surface drifted")
    if spec.capability_truth_scope != PUBLIC_BRIDGE_CAPABILITY_TRUTH_SCOPE:
        errors.append("capability truth scope drifted")
    if not spec.pass_through_args:
        errors.append("public bridge must pass through action arguments")

    retired_fields = sorted(
        field
        for field in fields
        if field.endswith("_scripts") or field in {"alternate_scripts", "old_scripts"}
    )
    if retired_fields:
        errors.append(
            "retired package metadata fields present: " + ", ".join(retired_fields)
        )
    return errors


def public_bridge_integrity(
    package_bridges: Mapping[str, PackageBridgeSpec],
) -> PublicBridgeIntegrity:
    errors: list[str] = []
    if len(package_bridges) != 1:
        errors.append(f"expected exactly one package bridge, found {len(package_bridges)}")
    for key, spec in package_bridges.items():
        errors.extend(_validate_bridge_spec(key, spec))

    return PublicBridgeIntegrity(
        contract_id=PUBLIC_BRIDGE_INTEGRITY_CONTRACT_ID,
        owner_surface=PUBLIC_BRIDGE_INTEGRITY_OWNER_SURFACE,
        package_bridge_count=len(package_bridges),
        canonical_package_bridge=WORKFLOW_BRIDGE_SCRIPT,
        canonical_invocation_template=WORKFLOW_PUBLIC_COMMAND_TEMPLATE,
        single_package_bridge_only=True,
        retired_metadata_allowed=False,
        pass_through_args_required=True,
        validation_errors=tuple(errors),
    )


def require_public_bridge_integrity(
    package_bridges: Mapping[str, PackageBridgeSpec],
) -> PublicBridgeIntegrity:
    integrity = public_bridge_integrity(package_bridges)
    if not integrity.valid:
        raise ValueError(
            "invalid objc3c public bridge registry: "
            + "; ".join(integrity.validation_errors)
        )
    return integrity


def public_bridge_integrity_fields(
    package_bridges: Mapping[str, PackageBridgeSpec],
) -> dict[str, object]:
    return asdict(public_bridge_integrity(package_bridges))


def package_bridge_spec(
    package_bridges: Mapping[str, PackageBridgeSpec],
    script_name: str,
) -> PackageBridgeSpec:
    require_public_bridge_integrity(package_bridges)
    try:
        return package_bridges[script_name]
    except KeyError as exc:
        raise KeyError(f"unknown objc3c package bridge: {script_name}") from exc


__all__ = [
    "PublicBridgeIntegrity",
    "package_bridge_spec",
    "public_bridge_integrity",
    "public_bridge_integrity_fields",
    "require_public_bridge_integrity",
]
