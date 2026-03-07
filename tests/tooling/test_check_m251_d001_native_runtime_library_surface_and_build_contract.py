from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m251_d001_native_runtime_library_surface_and_build_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m251_d001_native_runtime_library_surface_and_build_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m251_d001_native_runtime_library_surface_and_build_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m251-d001-native-runtime-library-surface-build-contract-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m251_d001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m251/M251-D001/")


def test_contract_fails_closed_when_expectations_drop_dispatch_symbol(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m251_native_runtime_library_surface_and_build_contract_d001_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`objc3_runtime_dispatch_i32`",
            "`objc3_runtime_dispatch_i64`",
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
    assert any(failure["check_id"] == "M251-D001-DOC-EXP-05" for failure in payload["failures"])


def test_contract_fails_closed_when_frontend_artifacts_drop_runtime_support_library_dispatch_key(
    tmp_path: Path,
) -> None:
    drift_artifacts = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_artifacts.write_text(
        contract.DEFAULT_FRONTEND_ARTIFACTS.read_text(encoding="utf-8").replace(
            "runtime_support_library_dispatch_i32_symbol",
            "runtime_support_library_dispatch_symbol",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--frontend-artifacts",
            str(drift_artifacts),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M251-D001-ART-03" for failure in payload["failures"])


def test_dynamic_probe_covers_manifest_and_ir_runtime_support_contract(tmp_path: Path) -> None:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native objc3c binary is not built")
    if contract.resolve_tool("llc.exe") is None:
        pytest.skip("llc.exe is not available")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 1

    case = payload["dynamic_cases"][0]
    assert case["case_id"] == "M251-D001-CASE-HELLO"
    assert case["process_exit_code"] == 0
    assert case["status"] == 0
    assert case["success"] is True
    assert case["runtime_support_library_contract_id"] == contract.CONTRACT_ID
    assert case["runtime_support_library_target_name"] == contract.TARGET_NAME
    assert case["runtime_support_library_public_header_path"] == contract.PUBLIC_HEADER
    assert case["runtime_support_library_register_image_symbol"] == contract.REGISTER_IMAGE
    assert case["runtime_support_library_lookup_selector_symbol"] == contract.LOOKUP_SELECTOR
    assert case["runtime_support_library_dispatch_i32_symbol"] == contract.DISPATCH_I32
    assert case["runtime_support_library_reset_for_testing_symbol"] == contract.RESET_FOR_TESTING
    assert case["runtime_support_library_driver_link_mode"] == contract.LINK_MODE
    assert case["runtime_support_library_shim_remains_test_only"] is True
    assert case["ir_contract_comment_present"] is True
    assert case["ir_named_metadata_present"] is True
