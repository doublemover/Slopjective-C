from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT / "scripts" / "check_m255_d001_lookup_and_dispatch_runtime_contract_and_architecture_freeze.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m255_d001_lookup_and_dispatch_runtime_contract_and_architecture_freeze",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m255_d001_lookup_and_dispatch_runtime_contract_and_architecture_freeze.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def _runtime_library_is_stale() -> bool:
    return bool(
        contract.stale_inputs(
            contract.DEFAULT_RUNTIME_LIBRARY,
            (
                contract.DEFAULT_RUNTIME_HEADER,
                contract.DEFAULT_RUNTIME_SOURCE,
                contract.DEFAULT_RUNTIME_PROBE,
            ),
        )
    )


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == (
        "m255-d001-lookup-dispatch-runtime-contract-and-architecture-freeze-v1"
    )
    assert payload["contract_id"] == contract.CONTRACT_ID
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m255_d001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m255/M255-D001/")


def test_contract_fails_closed_when_expectations_drop_contract_id(tmp_path: Path) -> None:
    drift_doc = tmp_path / contract.DEFAULT_EXPECTATIONS_DOC.name
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            contract.CONTRACT_ID,
            "objc3c-runtime-lookup-dispatch-freeze/m255-d001-drift",
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
        failure["check_id"] == "M255-D001-DOC-EXP-02"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_lowering_contract_drops_cache_model(
    tmp_path: Path,
) -> None:
    drift_header = tmp_path / contract.DEFAULT_LOWERING_CONTRACT.name
    drift_header.write_text(
        contract.DEFAULT_LOWERING_CONTRACT.read_text(encoding="utf-8").replace(
            "kObjc3RuntimeLookupDispatchCacheModel",
            "kObjc3RuntimeLookupDispatchLatencySurface",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--lowering-contract",
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
        failure["check_id"] == "M255-D001-LC-05"
        for failure in payload["failures"]
    )


def test_dynamic_probe_covers_runtime_lookup_and_dispatch_boundary(
    tmp_path: Path,
) -> None:
    if not contract.DEFAULT_RUNTIME_LIBRARY.exists():
        pytest.skip("runtime library is not built")
    if _runtime_library_is_stale():
        pytest.skip("runtime library is older than D001 runtime inputs")
    if contract.resolve_tool("clang++.exe") is None and contract.resolve_tool("clang++") is None:
        pytest.skip("clang++ is not available")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 1

    case = payload["dynamic_cases"][0]
    assert case["case_id"] == "runtime-probe"
    assert case["compile_exit_code"] == 0
    assert case["run_exit_code"] == 0

    probe_payload = case["probe_payload"]
    assert probe_payload["register_status"] == 0
    assert probe_payload["lookup_null_is_null"] is True
    assert probe_payload["copy_selector_reused"] is True
    assert probe_payload["copy_selector_stable_id"] == 1
    assert probe_payload["gamma_selector_stable_id"] == 2
    assert probe_payload["copy_selector_spelling_matches"] is True
    assert probe_payload["dispatch_result"] == contract.expected_dispatch(
        7, "copy", 1, 2, 3, 4
    )
    assert probe_payload["expected_dispatch_result"] == contract.expected_dispatch(
        7, "copy", 1, 2, 3, 4
    )
    assert probe_payload["nil_dispatch_result"] == 0
    assert probe_payload["snapshot_status"] == 0
    assert probe_payload["registered_image_count"] == 1
    assert probe_payload["registered_descriptor_total"] == 1
    assert probe_payload["next_expected_registration_order_ordinal"] == 2
    assert probe_payload["last_successful_registration_order_ordinal"] == 1
    assert probe_payload["last_registration_status"] == 0
    assert probe_payload["last_registered_module_name"] == "m255-d001-runtime-probe"
    assert (
        probe_payload["last_registered_translation_unit_identity_key"]
        == "m255-d001::translation-unit"
    )
    assert probe_payload["copy_after_reset_stable_id"] == 1
