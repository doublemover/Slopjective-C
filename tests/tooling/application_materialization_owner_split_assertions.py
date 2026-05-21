from __future__ import annotations

from typing import Any, Iterable

from application_materialization_owner_split_sources import (
    PROJECT_TEMPLATE_CONTRACT_ID,
    STDLIB_WORKSPACE_SUMMARY_CONTRACT_ID,
)


def assert_script_delegates_to_owner_symbols(
    script_text: str,
    owner_symbols: Iterable[str],
) -> None:
    assert "objc3c_application_materialization." in script_text
    for owner_symbol in owner_symbols:
        assert owner_symbol in script_text
    assert "json.dumps(" not in script_text
    assert "shutil." not in script_text


def assert_project_template_manifest_payload(payload: dict[str, Any]) -> None:
    assert payload["contract_id"] == PROJECT_TEMPLATE_CONTRACT_ID
    assert payload["template_source"] == (
        "tmp/artifacts/project-template/auroraBoard/src/main.objc3"
    )
    assert payload["application_architecture_testing_contracts"] == {
        "first_party_testing": "contracts/first.json",
        "project_template_workspace": "contracts/workspace.json",
        "canonical_application_architecture": "contracts/canonical.json",
    }
    assert payload["template_compile_contract"]["compile_action"] == "compile-objc3c"
    assert payload["template_compile_contract"]["artifact_root"] == (
        "tmp/artifacts/project-template/auroraBoard/build"
    )
    assert "compile-objc3c" in payload["public_actions"]


def assert_stdlib_workspace_summary_payload(payload: dict[str, Any]) -> None:
    assert payload["contract_id"] == STDLIB_WORKSPACE_SUMMARY_CONTRACT_ID
    assert payload["copied_file_count"] == 2
    assert payload["copied_paths"] == [
        "stdlib/core/main.objc3",
        "stdlib/workspace.json",
    ]
    assert payload["compatibility_gates"] == "stdlib/compatibility_gates.json"
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
