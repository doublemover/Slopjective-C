"""Shared action-name inventory helpers for public command facades."""

from __future__ import annotations

from collections.abc import Callable, Mapping
from dataclasses import asdict, dataclass
from typing import Any

from ..action_spec import ActionSpec
from ..public_bridge_constants import (
    PUBLIC_BRIDGE_PAYLOAD_OWNER_SURFACE,
    PUBLIC_BRIDGE_REGISTRY_OWNER_SURFACE,
    WORKFLOW_BRIDGE_SCRIPT,
    WORKFLOW_MODULE,
    WORKFLOW_PUBLIC_COMMAND_TEMPLATE,
    WORKFLOW_RUNNER_MODE,
    WORKFLOW_RUNNER_SURFACE,
)
from ..npm_surface import describe_package_script_payload
from ..public_bridge_registry import PACKAGE_BRIDGES
from ..registry_views import actions_by_category, actions_matching


COMMAND_FACADE_INVENTORY_CONTRACT_ID = "objc3c-workflow-command-facade-inventory-v1"
COMMAND_FACADE_INVENTORY_OWNER_SURFACE = (
    "scripts/objc3c_workflow/actions/command_facades_inventory.py"
)
COMMAND_FACADE_APPENDIX_GENERATOR = "scripts/render_objc3c_public_command_surface.py"


@dataclass(frozen=True)
class PackageBridgeInventory:
    contract_id: str
    owner_surface: str
    package_bridge_count: int
    canonical_package_bridge: str
    package_bridges: tuple[str, ...]
    missing_package_bridge: tuple[str, ...]
    unexpected_package_bridges: tuple[str, ...]
    single_package_bridge_only: bool
    runner_mode: str
    runner_path: str
    workflow_module: str
    public_command_template: str
    registry_owner_surface: str
    payload_owner_surface: str

    def fields(self) -> dict[str, object]:
        payload = asdict(self)
        payload["package_bridges"] = list(self.package_bridges)
        payload["missing_package_bridge"] = list(self.missing_package_bridge)
        payload["unexpected_package_bridges"] = list(self.unexpected_package_bridges)
        return payload


def category_action_names(category: str) -> list[str]:
    return actions_by_category(category)


def category_group_action_names(categories: tuple[str, ...]) -> list[str]:
    names: list[str] = []
    for category in categories:
        names.extend(actions_by_category(category))
    return names


def matching_action_names(predicate: Callable[[str, ActionSpec], bool]) -> list[str]:
    return actions_matching(predicate)


def package_bridge_names_from_scripts(scripts: Mapping[str, Any]) -> list[str]:
    return sorted(name for name in scripts if name in PACKAGE_BRIDGES)


def missing_package_bridge_names(scripts: Mapping[str, Any]) -> list[str]:
    present = set(package_bridge_names_from_scripts(scripts))
    return sorted(name for name in PACKAGE_BRIDGES if name not in present)


def unexpected_package_bridge_names(scripts: Mapping[str, Any]) -> list[str]:
    return sorted(name for name in scripts if name not in PACKAGE_BRIDGES)


def package_bridge_inventory(scripts: Mapping[str, Any]) -> PackageBridgeInventory:
    package_bridges = tuple(package_bridge_names_from_scripts(scripts))
    return PackageBridgeInventory(
        contract_id=COMMAND_FACADE_INVENTORY_CONTRACT_ID,
        owner_surface=COMMAND_FACADE_INVENTORY_OWNER_SURFACE,
        package_bridge_count=len(package_bridges),
        canonical_package_bridge=WORKFLOW_BRIDGE_SCRIPT,
        package_bridges=package_bridges,
        missing_package_bridge=tuple(missing_package_bridge_names(scripts)),
        unexpected_package_bridges=tuple(unexpected_package_bridge_names(scripts)),
        single_package_bridge_only=True,
        runner_mode=WORKFLOW_RUNNER_MODE,
        runner_path=WORKFLOW_RUNNER_SURFACE,
        workflow_module=WORKFLOW_MODULE,
        public_command_template=WORKFLOW_PUBLIC_COMMAND_TEMPLATE,
        registry_owner_surface=PUBLIC_BRIDGE_REGISTRY_OWNER_SURFACE,
        payload_owner_surface=PUBLIC_BRIDGE_PAYLOAD_OWNER_SURFACE,
    )


def package_bridge_inventory_fields(scripts: Mapping[str, Any]) -> dict[str, object]:
    return package_bridge_inventory(scripts).fields()


def package_bridge_payloads_from_scripts(
    scripts: Mapping[str, Any],
) -> list[dict[str, object]]:
    return [
        describe_package_script_payload(script_name)
        for script_name in package_bridge_names_from_scripts(scripts)
    ]


def public_command_orchestration_model() -> dict[str, str]:
    return {
        "package_bridge_owner": WORKFLOW_RUNNER_SURFACE,
        "package_bridge_registry_owner": PUBLIC_BRIDGE_REGISTRY_OWNER_SURFACE,
        "package_bridge_payload_owner": PUBLIC_BRIDGE_PAYLOAD_OWNER_SURFACE,
        "internal_action_owner": (
            "ACTION_SPECS actions are reached through the objc3c package bridge"
        ),
        "appendix_generator": COMMAND_FACADE_APPENDIX_GENERATOR,
        "inventory_owner": COMMAND_FACADE_INVENTORY_OWNER_SURFACE,
    }


def command_facade_inventory_contract(
    scripts: Mapping[str, Any],
    *,
    workflow_action_count: int,
    public_action_count: int,
    internal_action_count: int,
) -> dict[str, object]:
    return {
        **package_bridge_inventory_fields(scripts),
        "workflow_action_count": workflow_action_count,
        "public_action_count": public_action_count,
        "internal_action_count": internal_action_count,
        "orchestration_model": public_command_orchestration_model(),
    }


__all__ = [
    "COMMAND_FACADE_APPENDIX_GENERATOR",
    "COMMAND_FACADE_INVENTORY_CONTRACT_ID",
    "COMMAND_FACADE_INVENTORY_OWNER_SURFACE",
    "PackageBridgeInventory",
    "category_action_names",
    "category_group_action_names",
    "command_facade_inventory_contract",
    "matching_action_names",
    "missing_package_bridge_names",
    "package_bridge_inventory",
    "package_bridge_inventory_fields",
    "package_bridge_names_from_scripts",
    "package_bridge_payloads_from_scripts",
    "public_command_orchestration_model",
    "unexpected_package_bridge_names",
]
