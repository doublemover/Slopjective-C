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
    / "check_m254_d003_realization_sequencing_and_deterministic_reset_hooks_core_feature_expansion.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m254_d003_realization_sequencing_and_deterministic_reset_hooks_core_feature_expansion",
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


def test_contract_default_summary_out_is_under_tmp_reports_m254_d003() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m254/M254-D003/")


def test_contract_fails_closed_when_internal_header_drops_replay_symbol(tmp_path: Path) -> None:
    drift_header = tmp_path / contract.DEFAULT_INTERNAL_HEADER.name
    drift_header.write_text(
        contract.DEFAULT_INTERNAL_HEADER.read_text(encoding="utf-8").replace(
            contract.REPLAY_SYMBOL,
            "objc3_runtime_replay_registered_images_for_manual_probe",
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
        failure["check_id"] == "M254-D003-INTH-02"
        for failure in payload["failures"]
    )


def test_dynamic_probe_covers_same_process_reset_replay(tmp_path: Path) -> None:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native frontend is not built")
    if _native_is_stale():
        pytest.skip("native frontend is older than D003 source inputs")
    if not contract.DEFAULT_RUNTIME_LIBRARY.exists():
        pytest.skip("runtime library is not built")
    if _runtime_library_is_stale():
        pytest.skip("runtime library is older than D003 source inputs")
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
        assert probe_payload["startup_registered_image_count"] == 1
        assert probe_payload["post_reset_registered_image_count"] == 0
        assert probe_payload["post_reset_last_reset_cleared_image_local_init_state_count"] >= 1
        assert probe_payload["replay_status"] == 0
        assert probe_payload["post_replay_registered_image_count"] == 1
        assert probe_payload["post_replay_last_registration_used_staged_table"] == 1
        assert probe_payload["replay_known_selector_stable_id"] == probe_payload[
            "startup_known_selector_stable_id"
        ]
