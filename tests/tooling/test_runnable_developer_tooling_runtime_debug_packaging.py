from __future__ import annotations

import json
import sys
from pathlib import Path
from types import SimpleNamespace
from typing import Any

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_tooling.json_io import require_json_object as load_json
from scripts.check_objc3c_runnable_developer_tooling_end_to_end.constants import (
    CONTRACT_PATH,
    PACKAGE_CONTRACT_ID,
)
from scripts.check_objc3c_runnable_developer_tooling_end_to_end.manifest import (
    PACKAGED_FILE_FIELDS,
    validate_manifest_contract,
)
from scripts.check_objc3c_runnable_developer_tooling_end_to_end.tooling import (
    run_integrated_validation_check,
    run_workspace_check,
)
import scripts.check_objc3c_runnable_developer_tooling_end_to_end.tooling as tooling_module


def _write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2), encoding="utf-8")


def _contract() -> dict[str, Any]:
    return load_json(CONTRACT_PATH)


def _touch_package_file(package_root: Path, relative_path: str) -> None:
    path = package_root / relative_path
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("", encoding="utf-8")


def _manifest_for_contract(package_root: Path, contract: dict[str, Any]) -> dict[str, Any]:
    manifest: dict[str, Any] = {
        "contract_id": PACKAGE_CONTRACT_ID,
        "developer_tooling_scripts": {
            "runtime_debug_trace": "scripts/build_objc3c_runtime_debug_trace.py",
            "runnable_end_to_end_validation": "scripts/check_objc3c_runnable_developer_tooling_end_to_end.py",
        },
        "developer_tooling_public_actions": list(contract["public_actions"]),
        "command_surfaces": {
            command_name: f"npm run objc3c -- {command_name}"
            for command_name in contract["required_command_surfaces"]
        },
        "package_bridge": contract["package_bridge"],
    }
    manifest["command_surfaces"]["runtime_debug_trace"] = contract[
        "expected_runtime_debug_trace_command_surface"
    ]
    for field in contract["manifest_fields"]:
        manifest.setdefault(field, f"packaged/{field}.json")
    manifest["developer_tooling_runtime_debug_trace_script"] = (
        "scripts/build_objc3c_runtime_debug_trace.py"
    )
    manifest["developer_tooling_runtime_debug_trace_schema"] = (
        contract["expected_runtime_debug_trace_schema"]
    )
    manifest["developer_tooling_runtime_debug_trace_path"] = (
        contract["expected_runtime_debug_trace_path"]
    )
    manifest["developer_tooling_runtime_debug_trace_model"] = (
        contract["expected_runtime_debug_trace_model"]
    )
    for field in PACKAGED_FILE_FIELDS:
        _touch_package_file(package_root, str(manifest[field]))
    for relative_path in manifest["developer_tooling_scripts"].values():
        _touch_package_file(package_root, str(relative_path))
    return manifest


def _runtime_debug_fields(contract: dict[str, Any]) -> dict[str, str]:
    return {
        "runtime_debug_trace_command": contract["expected_runtime_debug_trace_command"],
        "runtime_debug_trace_path": contract["expected_runtime_debug_trace_path"],
        "runtime_debug_trace_schema": contract["expected_runtime_debug_trace_schema"],
        "runtime_debug_trace_model": contract["expected_runtime_debug_trace_model"],
    }


def test_packaged_manifest_contract_requires_runtime_debug_trace_surface(tmp_path: Path) -> None:
    contract = _contract()
    manifest = _manifest_for_contract(tmp_path, contract)

    validate_manifest_contract(
        manifest=manifest,
        contract=contract,
        package_root=tmp_path,
    )

    assert "trace-runtime-debug" in manifest["developer_tooling_public_actions"]
    assert "runtime_debug_trace" in manifest["command_surfaces"]
    assert manifest["developer_tooling_runtime_debug_trace_script"] == (
        "scripts/build_objc3c_runtime_debug_trace.py"
    )
    assert manifest["developer_tooling_runtime_debug_trace_schema"] == (
        "schemas/objc3c-runtime-debug-trace-v1.schema.json"
    )


def test_packaged_manifest_contract_rejects_missing_runtime_debug_trace_surface(
    tmp_path: Path,
) -> None:
    contract = _contract()
    manifest = _manifest_for_contract(tmp_path, contract)
    manifest["command_surfaces"].pop("runtime_debug_trace")

    with pytest.raises(RuntimeError, match="runtime_debug_trace"):
        validate_manifest_contract(
            manifest=manifest,
            contract=contract,
            package_root=tmp_path,
        )


def test_packaged_workspace_check_requires_runtime_debug_trace_payload(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    contract = _contract()
    workspace_path = tmp_path / "workspace.json"
    _write_json(
        workspace_path,
        {
            "editor_tooling": {
                "debugger_model": contract["expected_debugger_model"],
                "statement_level_stepping": False,
                "workspace_index_guardrails_ok": True,
                "workspace_package_count": 9,
                **_runtime_debug_fields(contract),
            },
            "public_actions": [
                "inspect-editor-tooling",
                "format-objc3c",
                "trace-runtime-debug",
                "validate-developer-tooling",
            ],
        },
    )
    monkeypatch.setattr(
        tooling_module,
        "run_materialize_playground_workspace",
        lambda source_path, *, cwd: SimpleNamespace(
            returncode=0,
            stdout="workspace_path: workspace.json\n",
        ),
    )

    run_workspace_check(
        package_root=tmp_path,
        hello_source=tmp_path / "hello.objc3",
        contract=contract,
    )


def test_packaged_workspace_check_rejects_missing_runtime_debug_trace_schema(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    contract = _contract()
    runtime_debug_fields = _runtime_debug_fields(contract)
    runtime_debug_fields.pop("runtime_debug_trace_schema")
    _write_json(
        tmp_path / "workspace.json",
        {
            "editor_tooling": {
                "debugger_model": contract["expected_debugger_model"],
                "statement_level_stepping": False,
                "workspace_index_guardrails_ok": True,
                "workspace_package_count": 9,
                **runtime_debug_fields,
            },
            "public_actions": [
                "inspect-editor-tooling",
                "format-objc3c",
                "trace-runtime-debug",
                "validate-developer-tooling",
            ],
        },
    )
    monkeypatch.setattr(
        tooling_module,
        "run_materialize_playground_workspace",
        lambda source_path, *, cwd: SimpleNamespace(
            returncode=0,
            stdout="workspace_path: workspace.json\n",
        ),
    )

    with pytest.raises(RuntimeError, match="runtime debug trace schema"):
        run_workspace_check(
            package_root=tmp_path,
            hello_source=tmp_path / "hello.objc3",
            contract=contract,
        )


def test_packaged_integrated_validation_requires_trace_runtime_debug_report(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    contract = _contract()
    summary_path = tmp_path / "integration-summary.json"
    runtime_debug_trace_path = tmp_path / contract["expected_runtime_debug_trace_path"]
    _write_json(
        summary_path,
        {
            "ok": True,
            "steps": [{"name": "trace-runtime-debug", "exit_code": 0}],
            "reports": {
                "runtime_debug_trace": contract["expected_runtime_debug_trace_path"],
            },
        },
    )
    _write_json(
        runtime_debug_trace_path,
        {
            "contract_id": "objc3c.runtime.debug.trace.v1",
            "ok": True,
        },
    )
    monkeypatch.setattr(
        tooling_module,
        "run_validate_developer_tooling",
        lambda *, cwd: SimpleNamespace(
            returncode=0,
            stdout="summary_path: integration-summary.json\n",
        ),
    )

    run_integrated_validation_check(package_root=tmp_path, contract=contract)


def test_packaged_integrated_validation_rejects_missing_trace_runtime_debug_step(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    contract = _contract()
    _write_json(
        tmp_path / "integration-summary.json",
        {
            "ok": True,
            "steps": [{"name": "inspect-editor-tooling", "exit_code": 0}],
            "reports": {
                "runtime_debug_trace": contract["expected_runtime_debug_trace_path"],
            },
        },
    )
    monkeypatch.setattr(
        tooling_module,
        "run_validate_developer_tooling",
        lambda *, cwd: SimpleNamespace(
            returncode=0,
            stdout="summary_path: integration-summary.json\n",
        ),
    )

    with pytest.raises(RuntimeError, match="trace-runtime-debug"):
        run_integrated_validation_check(package_root=tmp_path, contract=contract)
