from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_application_materialization.contract_models import (
    PROJECT_TEMPLATE_CONTRACT_ID,
    STDLIB_WORKSPACE_SUMMARY_CONTRACT_ID,
    StdlibModuleMaterialization,
    StdlibWorkspaceMaterialization,
)
from scripts.objc3c_application_materialization.manifests import (
    project_template_manifest_payload,
    stdlib_workspace_summary_payload,
)
from scripts.objc3c_application_materialization.copy_materialization import (
    project_template_paths,
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


def test_materialization_owner_modules_are_explicit() -> None:
    for module_name in OWNER_MODULES:
        assert importlib.import_module(module_name)


def test_materialization_entrypoints_delegate_to_owner_modules() -> None:
    for relative_path, owner_symbols in ENTRYPOINT_OWNER_IMPORTS.items():
        script_text = (ROOT / relative_path).read_text(encoding="utf-8")
        assert "objc3c_application_materialization." in script_text
        for owner_symbol in owner_symbols:
            assert owner_symbol in script_text
        assert "json.dumps(" not in script_text
        assert "shutil." not in script_text


def test_project_template_manifest_is_owned_by_manifest_builder(tmp_path: Path) -> None:
    paths = project_template_paths(
        artifact_root=tmp_path / "tmp" / "artifacts" / "project-template",
        report_root=tmp_path / "tmp" / "reports" / "project-template",
        example_id="auroraBoard",
    )
    payload = project_template_manifest_payload(
        root=tmp_path,
        example_id="auroraBoard",
        example_record={"source": "showcase/auroraBoard/main.objc3"},
        paths=paths,
        application_architecture_contracts={
            "first_party_testing": tmp_path / "contracts" / "first.json",
            "project_template_workspace": tmp_path / "contracts" / "workspace.json",
            "canonical_application_architecture": tmp_path / "contracts" / "canonical.json",
        },
    )

    assert payload["contract_id"] == PROJECT_TEMPLATE_CONTRACT_ID
    assert payload["template_source"] == (
        "tmp/artifacts/project-template/auroraBoard/src/main.objc3"
    )
    assert payload["application_architecture_testing_contracts"] == {
        "first_party_testing": "contracts/first.json",
        "project_template_workspace": "contracts/workspace.json",
        "canonical_application_architecture": "contracts/canonical.json",
    }


def test_stdlib_summary_contract_dedupes_materialized_paths(tmp_path: Path) -> None:
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

    payload = stdlib_workspace_summary_payload(
        root=tmp_path,
        workspace_path=tmp_path / "stdlib" / "workspace.json",
        workspace={
            "module_inventory": "stdlib/module_inventory.json",
            "stability_policy": "stdlib/stability_policy.json",
            "package_surface": "stdlib/package_surface.json",
        },
        materialization=materialization,
    )

    assert payload["contract_id"] == STDLIB_WORKSPACE_SUMMARY_CONTRACT_ID
    assert payload["copied_file_count"] == 2
    assert payload["copied_paths"] == [
        "stdlib/core/main.objc3",
        "stdlib/workspace.json",
    ]
    assert payload["modules"] == [
        {
            "canonical_module": "objc3.core",
            "implementation_module": "ObjC3Core",
            "workspace_root": "stdlib/core",
            "manifest": "stdlib/core/module.json",
            "source": "stdlib/core/main.objc3",
            "smoke_source": "stdlib/core/smoke.objc3",
        }
    ]
