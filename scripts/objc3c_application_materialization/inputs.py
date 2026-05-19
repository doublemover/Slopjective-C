"""Input loading for application and stdlib materializers."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_any, load_json_object


@dataclass(frozen=True)
class CanonicalApplicationInputs:
    contract: dict[str, object]
    portfolio: dict[str, object]
    stdlib_program_surface: dict[str, object]


@dataclass(frozen=True)
class StdlibWorkspaceInputs:
    workspace: dict[str, Any]
    inventory: dict[str, Any]
    stability_policy: Any
    package_surface: Any


def load_canonical_application_inputs(
    *,
    contract_path: Path,
    portfolio_path: Path,
    stdlib_program_surface_path: Path,
) -> CanonicalApplicationInputs:
    return CanonicalApplicationInputs(
        contract=load_json_object(contract_path),
        portfolio=load_json_object(portfolio_path),
        stdlib_program_surface=load_json_object(stdlib_program_surface_path),
    )


def load_stdlib_workspace_inputs(*, root: Path, workspace_path: Path) -> StdlibWorkspaceInputs:
    workspace = load_json_any(workspace_path)
    if not isinstance(workspace, dict):
        raise TypeError(f"stdlib workspace must be a JSON object: {workspace_path}")
    return StdlibWorkspaceInputs(
        workspace=workspace,
        inventory=_load_workspace_object(root, workspace, "module_inventory"),
        stability_policy=load_json_any(root / str(workspace["stability_policy"])),
        package_surface=load_json_any(root / str(workspace["package_surface"])),
    )


def application_architecture_contract_paths(root: Path) -> dict[str, Path]:
    application_architecture_root = (
        root / "tests" / "tooling" / "fixtures" / "application_architecture_testing"
    )
    return {
        "first_party_testing": application_architecture_root
        / "first_party_testing_semantics.json",
        "project_template_workspace": application_architecture_root
        / "project_template_workspace_semantics.json",
        "canonical_application_architecture": application_architecture_root
        / "canonical_application_architecture_semantics.json",
    }


def _load_workspace_object(
    root: Path, workspace: dict[str, Any], workspace_key: str
) -> dict[str, Any]:
    payload = load_json_any(root / str(workspace[workspace_key]))
    if not isinstance(payload, dict):
        raise TypeError(f"stdlib {workspace_key} must be a JSON object")
    return payload
