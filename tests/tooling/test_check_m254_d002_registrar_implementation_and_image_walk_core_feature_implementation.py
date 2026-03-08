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
    / "check_m254_d002_registrar_implementation_and_image_walk_core_feature_implementation.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m254_d002_registrar_implementation_and_image_walk_core_feature_implementation",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(f"Unable to load {SCRIPT_PATH}")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def _native_is_stale() -> bool:
    return bool(
        contract.stale_inputs(
            contract.DEFAULT_NATIVE_EXE,
            (
                contract.DEFAULT_INTERNAL_HEADER,
                contract.DEFAULT_RUNTIME_SOURCE,
                contract.DEFAULT_IR_EMITTER,
                contract.DEFAULT_FRONTEND_ARTIFACTS,
                contract.DEFAULT_DRIVER_CPP,
                contract.DEFAULT_PROCESS_HEADER,
                contract.DEFAULT_PROCESS_CPP,
                contract.DEFAULT_FRONTEND_ANCHOR_CPP,
            ),
        )
    )



def _runtime_library_is_stale() -> bool:
    return bool(
        contract.stale_inputs(
            contract.DEFAULT_RUNTIME_LIBRARY,
            (contract.DEFAULT_INTERNAL_HEADER, contract.DEFAULT_RUNTIME_SOURCE),
        )
    )



def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == contract.MODE
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []



def test_contract_default_summary_out_is_under_tmp_reports_m254_d002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m254/M254-D002/")



def test_contract_fails_closed_when_internal_header_drops_stage_symbol(
    tmp_path: Path,
) -> None:
    drift_header = tmp_path / contract.DEFAULT_INTERNAL_HEADER.name
    drift_header.write_text(
        contract.DEFAULT_INTERNAL_HEADER.read_text(encoding="utf-8").replace(
            contract.STAGE_REGISTRATION_TABLE_SYMBOL,
            "objc3_runtime_stage_registration_table_for_manual_probe",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--internal-header",
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
        failure["check_id"] == "M254-D002-INTH-02"
        for failure in payload["failures"]
    )



def test_dynamic_probe_covers_startup_image_walk(tmp_path: Path) -> None:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native frontend is not built")
    if _native_is_stale():
        pytest.skip("native frontend is older than D002 source inputs")
    if not contract.DEFAULT_RUNTIME_LIBRARY.exists():
        pytest.skip("runtime library is not built")
    if _runtime_library_is_stale():
        pytest.skip("runtime library is older than D002 source inputs")
    if contract.resolve_tool("clang++.exe") is None and contract.resolve_tool("clang++") is None:
        pytest.skip("clang++ is not available")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == len(contract.DYNAMIC_CASES)
    for case in payload["dynamic_cases"]:
        probe_payload = case["probe_payload"]
        assert probe_payload["registration_copy_status"] == 0
        assert probe_payload["image_walk_copy_status"] == 0
        assert probe_payload["walked_image_count"] == 1
        assert probe_payload["last_registration_used_staged_table"] == 1
        assert probe_payload["last_linker_anchor_matches_discovery_root"] == 1
        assert probe_payload["known_selector_stable_id"] > 0
        assert probe_payload["unknown_selector_stable_id"] > probe_payload[
            "last_walked_selector_pool_count"
        ]
