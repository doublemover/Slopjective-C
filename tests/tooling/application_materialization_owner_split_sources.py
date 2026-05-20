from __future__ import annotations

import importlib
from pathlib import Path
from types import ModuleType
from typing import Any

from scripts.objc3c_application_materialization.contract_models import (
    PROJECT_TEMPLATE_CONTRACT_ID,
    STDLIB_WORKSPACE_SUMMARY_CONTRACT_ID,
    StdlibModuleMaterialization,
    StdlibWorkspaceMaterialization,
)
from scripts.objc3c_application_materialization.copy_materialization import (
    project_template_paths,
)
from scripts.objc3c_application_materialization.manifests import (
    project_template_manifest_payload,
    stdlib_workspace_summary_payload,
)

ROOT = Path(__file__).resolve().parents[2]

OWNER_MODULES = (
    "scripts.objc3c_application_materialization.inputs",
    "scripts.objc3c_application_materialization.manifests",
    "scripts.objc3c_application_materialization.copy_materialization",
    "scripts.objc3c_application_materialization.contract_models",
    "scripts.objc3c_application_materialization.result_rendering",
)

ENTRYPOINT_OWNER_IMPORTS = {
    "scripts/materialize_objc3c_canonical_application_workspace.py": (
        "load_canonical_application_inputs",
        "materialize_canonical_workspace",
        "canonical_workspace_manifest_payload",
        "print_canonical_workspace_result",
    ),
    "scripts/materialize_objc3c_project_template.py": (
        "application_architecture_contract_paths",
        "materialize_project_template_source",
        "project_template_manifest_payload",
        "print_project_template_result",
    ),
    "scripts/materialize_objc3c_stdlib_workspace.py": (
        "load_stdlib_workspace_inputs",
        "materialize_stdlib_workspace",
        "stdlib_workspace_summary_payload",
        "print_stdlib_workspace_result",
    ),
}


def owner_module_names() -> tuple[str, ...]:
    return OWNER_MODULES


def import_owner_module(module_name: str) -> ModuleType:
    return importlib.import_module(module_name)


def entrypoint_owner_imports() -> dict[str, tuple[str, ...]]:
    return ENTRYPOINT_OWNER_IMPORTS


def entrypoint_script_text(relative_path: str) -> str:
    return (ROOT / relative_path).read_text(encoding="utf-8")


def project_template_manifest_for_tmp(tmp_path: Path) -> dict[str, Any]:
    paths = project_template_paths(
        artifact_root=tmp_path / "tmp" / "artifacts" / "project-template",
        report_root=tmp_path / "tmp" / "reports" / "project-template",
        example_id="auroraBoard",
    )
    return project_template_manifest_payload(
        root=tmp_path,
        example_id="auroraBoard",
        example_record={"source": "showcase/auroraBoard/main.objc3"},
        paths=paths,
        application_architecture_contracts={
            "first_party_testing": tmp_path / "contracts" / "first.json",
            "project_template_workspace": tmp_path / "contracts" / "workspace.json",
            "canonical_application_architecture": tmp_path
            / "contracts"
            / "canonical.json",
        },
    )


def stdlib_workspace_summary_for_tmp(tmp_path: Path) -> dict[str, Any]:
    materialization = StdlibWorkspaceMaterialization(
        output_root=tmp_path / "out",
        summary_path=tmp_path / "out" / "stdlib.materialized.workspace.json",
        copied_paths=[
            "stdlib/workspace.json",
            "stdlib/workspace.json",
            "stdlib/core/main.objc3",
        ],
        modules=[
            StdlibModuleMaterialization(
                canonical_module="objc3.core",
                implementation_module="ObjC3Core",
                workspace_root="stdlib/core",
                manifest="stdlib/core/module.json",
                source="stdlib/core/main.objc3",
                smoke_source="stdlib/core/smoke.objc3",
            )
        ],
    )

    return stdlib_workspace_summary_payload(
        root=tmp_path,
        workspace_path=tmp_path / "stdlib" / "workspace.json",
        workspace={
            "module_inventory": "stdlib/module_inventory.json",
            "stability_policy": "stdlib/stability_policy.json",
            "package_surface": "stdlib/package_surface.json",
            "compatibility_gates": "stdlib/compatibility_gates.json",
        },
        materialization=materialization,
    )
