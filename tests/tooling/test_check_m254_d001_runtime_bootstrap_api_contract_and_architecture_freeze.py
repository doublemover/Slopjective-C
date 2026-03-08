from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT / "scripts" / "check_m254_d001_runtime_bootstrap_api_contract_and_architecture_freeze.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m254_d001_runtime_bootstrap_api_contract_and_architecture_freeze",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m254_d001_runtime_bootstrap_api_contract_and_architecture_freeze.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def _native_is_stale() -> bool:
    return bool(
        contract.stale_inputs(
            contract.DEFAULT_NATIVE_EXE,
            (
                contract.DEFAULT_FRONTEND_ARTIFACTS,
                contract.DEFAULT_FRONTEND_ARTIFACTS_H,
                contract.DEFAULT_FRONTEND_TYPES,
                contract.DEFAULT_DRIVER_CPP,
                contract.DEFAULT_PROCESS_H,
                contract.DEFAULT_PROCESS_CPP,
                contract.DEFAULT_RUNTIME_HEADER,
                contract.DEFAULT_RUNTIME_SOURCE,
                contract.DEFAULT_RUNTIME_PROBE,
            ),
        )
    )


def _runtime_library_is_stale() -> bool:
    return bool(
        contract.stale_inputs(
            contract.DEFAULT_RUNTIME_LIBRARY,
            (
                contract.DEFAULT_RUNTIME_HEADER,
                contract.DEFAULT_RUNTIME_SOURCE,
            ),
        )
    )


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == (
        "m254-d001-runtime-bootstrap-api-contract-and-architecture-freeze-v1"
    )
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 60
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m254_d001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m254/M254-D001/")


def test_contract_fails_closed_when_expectations_drop_surface_path(tmp_path: Path) -> None:
    drift_doc = tmp_path / contract.DEFAULT_EXPECTATIONS_DOC.name
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            contract.SURFACE_PATH,
            "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_api_surface",
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
    assert any(
        failure["check_id"] == "M254-D001-DOC-EXP-03"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_runtime_header_drops_reset_symbol(
    tmp_path: Path,
) -> None:
    drift_header = tmp_path / contract.DEFAULT_RUNTIME_HEADER.name
    drift_header.write_text(
        contract.DEFAULT_RUNTIME_HEADER.read_text(encoding="utf-8").replace(
            contract.RESET_FOR_TESTING_SYMBOL,
            "objc3_runtime_reset_for_repl_testing",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--runtime-header",
            str(drift_header),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M254-D001-RTH-09"
        for failure in payload["failures"]
    )


def test_dynamic_probe_covers_manifest_and_runtime_api(tmp_path: Path) -> None:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native frontend is not built")
    if _native_is_stale():
        pytest.skip("native frontend is older than D001 source inputs")
    if not contract.DEFAULT_RUNTIME_LIBRARY.exists():
        pytest.skip("runtime library is not built")
    if _runtime_library_is_stale():
        pytest.skip("runtime library is older than runtime source inputs")
    if contract.resolve_tool("clang++.exe") is None and contract.resolve_tool("clang++") is None:
        pytest.skip("clang++ is not available")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 2

    cases = {case["case_id"]: case for case in payload["dynamic_cases"]}
    assert set(cases) == {contract.HELLO_FIXTURE_CASE_ID, contract.PROBE_CASE_ID}

    manifest_case = cases[contract.HELLO_FIXTURE_CASE_ID]
    assert manifest_case["process_exit_code"] == 0
    assert manifest_case["packet_contract_id"] == contract.CONTRACT_ID
    assert manifest_case["registration_manifest_contract_id"] == contract.CONTRACT_ID
    assert manifest_case["packet_replay_key"]
    assert manifest_case["flat_replay_key"]

    probe_case = cases[contract.PROBE_CASE_ID]
    assert probe_case["compile_exit_code"] == 0
    assert probe_case["run_exit_code"] == 0
    probe_payload = probe_case["probe_payload"]
    assert probe_payload["register_status"] == 0
    assert probe_payload["selector_stable_id"] == 1
    assert probe_payload["dispatch_result"] == contract.expected_dispatch(
        5, "bootstrap:ready:", 1, 2, 3, 4
    )
    assert probe_payload["post_register_registered_image_count"] == 1
    assert probe_payload["post_register_registered_descriptor_total"] == 5
    assert probe_payload["post_reset_registered_image_count"] == 0
    assert probe_payload["selector_after_reset_stable_id"] == 1
