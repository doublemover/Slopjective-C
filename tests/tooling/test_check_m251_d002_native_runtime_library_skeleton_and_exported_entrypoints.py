from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m251_d002_native_runtime_library_skeleton_and_exported_entrypoints.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m251_d002_native_runtime_library_skeleton_and_exported_entrypoints",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m251_d002_native_runtime_library_skeleton_and_exported_entrypoints.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m251-d002-native-runtime-library-core-feature-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 50
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m251_d002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m251/M251-D002/")


def test_contract_fails_closed_when_expectations_drop_archive_path(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m251_native_runtime_library_core_feature_d002_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`artifacts/lib/objc3_runtime.lib`",
            "`artifacts/lib/objc3_runtime_fake.lib`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--expectations-doc",
            str(drift_doc),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M251-D002-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_build_script_drops_runtime_archive_step(tmp_path: Path) -> None:
    drift_script = tmp_path / "build_objc3c_native.ps1"
    drift_script.write_text(
        contract.DEFAULT_BUILD_SCRIPT.read_text(encoding="utf-8").replace(
            'Write-BuildStep ("archive_start=objc3_runtime -> "',
            'Write-BuildStep ("archive_begin=objc3_runtime_missing -> "',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--build-script",
            str(drift_script),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M251-D002-BLD-03" for failure in payload["failures"])


def test_dynamic_probe_covers_manifest_ir_and_runtime_archive(tmp_path: Path) -> None:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native objc3c binary is not built")
    if not contract.DEFAULT_RUNTIME_LIBRARY.exists():
        pytest.skip("native runtime library archive is not built")
    if contract.resolve_tool("llc.exe") is None:
        pytest.skip("llc.exe is not available")
    if contract.resolve_tool("clang++.exe") is None and contract.resolve_tool("clang++") is None:
        pytest.skip("clang++ is not available")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 2

    hello_case = payload["dynamic_cases"][0]
    assert hello_case["case_id"] == "M251-D002-CASE-HELLO"
    assert hello_case["process_exit_code"] == 0
    assert hello_case["status"] == 0
    assert hello_case["success"] is True
    assert hello_case["runtime_support_library_core_feature_contract_id"] == contract.CONTRACT_ID
    assert hello_case["runtime_support_library_core_feature_archive_relative_path"] == contract.ARCHIVE_RELATIVE_PATH
    assert hello_case["runtime_support_library_core_feature_probe_source_path"] == contract.PROBE_SOURCE_PATH
    assert hello_case["runtime_support_library_core_feature_dispatch_i32_symbol"] == contract.DISPATCH_I32
    assert hello_case["runtime_support_library_core_feature_driver_link_mode"] == contract.LINK_MODE
    assert hello_case["runtime_support_library_core_feature_entrypoints_implemented"] is True
    assert hello_case["ir_contract_comment_present"] is True
    assert hello_case["ir_named_metadata_present"] is True

    probe_case = payload["dynamic_cases"][1]
    assert probe_case["case_id"] == "M251-D002-CASE-PROBE"
    assert probe_case["runtime_library_exists"] is True
    assert probe_case["compile_exit_code"] == 0
    assert probe_case["run_exit_code"] == 0
    assert probe_case["status"] == 0
    assert probe_case["success"] is True
