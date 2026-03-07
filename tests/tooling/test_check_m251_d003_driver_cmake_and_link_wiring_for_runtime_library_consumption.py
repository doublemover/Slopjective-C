from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m251_d003_driver_cmake_and_link_wiring_for_runtime_library_consumption.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m251_d003_driver_cmake_and_link_wiring_for_runtime_library_consumption",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(f"Unable to load {SCRIPT_PATH}")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m251-d003-runtime-library-link-wiring-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 35
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m251_d003() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m251/M251-D003/")


def test_contract_fails_closed_when_runtime_alias_is_removed(tmp_path: Path) -> None:
    drift_source = tmp_path / "objc3_runtime.cpp"
    drift_source.write_text(
        contract.DEFAULT_RUNTIME_SOURCE.read_text(encoding="utf-8").replace(
            'extern "C" int objc3_msgsend_i32(',
            'extern "C" int objc3_msgsend_i32_removed(',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--runtime-source",
            str(drift_source),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M251-D003-RTS-01" for failure in payload["failures"])


def test_contract_fails_closed_when_smoke_script_stops_consuming_manifest_runtime_archive(tmp_path: Path) -> None:
    drift_script = tmp_path / "check_objc3c_native_execution_smoke.ps1"
    drift_script.write_text(
        contract.DEFAULT_SMOKE_SCRIPT.read_text(encoding="utf-8").replace(
            "runtime_support_library_link_wiring_archive_relative_path",
            "runtime_support_library_link_contract_archive_path_removed",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--smoke-script",
            str(drift_script),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M251-D003-SMK-02" for failure in payload["failures"])


def test_dynamic_smoke_proves_runtime_archive_consumption(tmp_path: Path) -> None:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native objc3c binary is not built")
    if not contract.DEFAULT_RUNTIME_LIBRARY.exists():
        pytest.skip("native runtime library archive is not built")
    if contract.resolve_tool("llc.exe", "llc") is None:
        pytest.skip("llc is not available")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 1
    case = payload["dynamic_cases"][0]
    assert case["case_id"] == "M251-D003-CASE-SMOKE"
    assert case["exit_code"] == 0
    assert case["summary_status"] == "PASS"
    assert case["runtime_library"] == contract.ARCHIVE_RELATIVE_PATH
    assert case["positive_runtime_fixture"].endswith(".objc3")
    assert case["negative_unresolved_fixture"].endswith("runtime_dispatch_unresolved_symbol.objc3")
    assert case["success"] is True
