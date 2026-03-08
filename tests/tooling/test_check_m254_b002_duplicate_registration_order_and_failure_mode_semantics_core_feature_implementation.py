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
    / "check_m254_b002_duplicate_registration_order_and_failure_mode_semantics_core_feature_implementation.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m254_b002_duplicate_registration_order_and_failure_mode_semantics_core_feature_implementation",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(f"Unable to load {SCRIPT_PATH}")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def _native_is_stale() -> bool:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        return True
    native_mtime = contract.DEFAULT_NATIVE_EXE.stat().st_mtime
    freshness_inputs = (
        contract.DEFAULT_AST_HEADER,
        contract.DEFAULT_FRONTEND_TYPES,
        contract.DEFAULT_FRONTEND_ARTIFACTS_HEADER,
        contract.DEFAULT_FRONTEND_ARTIFACTS,
        contract.DEFAULT_DRIVER_CPP,
        contract.DEFAULT_PROCESS_HEADER,
        contract.DEFAULT_PROCESS_CPP,
        contract.DEFAULT_FRONTEND_ANCHOR_CPP,
        contract.DEFAULT_RUNTIME_HEADER,
        contract.DEFAULT_RUNTIME_SOURCE,
        contract.DEFAULT_RUNTIME_PROBE,
    )
    return any(path.stat().st_mtime > native_mtime for path in freshness_inputs)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m254-b002-live-bootstrap-semantics-core-feature-implementation-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 55
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m254_b002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m254/M254-B002/")


def test_contract_fails_closed_when_expectations_drop_snapshot_symbol(tmp_path: Path) -> None:
    drift_doc = tmp_path / contract.DEFAULT_EXPECTATIONS_DOC.name
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`objc3_runtime_copy_registration_state_for_testing`",
            "`objc3_runtime_snapshot_for_testing_v2`",
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
        failure["check_id"] == "M254-B002-DOC-EXP-04"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_runtime_source_drops_out_of_order_status(tmp_path: Path) -> None:
    drift_runtime = tmp_path / "objc3_runtime.cpp"
    drift_runtime.write_text(
        contract.DEFAULT_RUNTIME_SOURCE.read_text(encoding="utf-8").replace(
            "registration_order_by_identity_key",
            "registration_order_by_image_key",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--runtime-source",
            str(drift_runtime),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M254-B002-RTS-02"
        for failure in payload["failures"]
    )


def test_dynamic_probe_covers_live_bootstrap_semantics(tmp_path: Path) -> None:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native frontend is not built")
    if _native_is_stale():
        pytest.skip("native frontend is older than the B002 source inputs")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 1

    case = payload["dynamic_cases"][0]
    assert case["fixture"] == "tests/tooling/fixtures/native/hello.objc3"
    assert case["registration_manifest_path"].endswith(
        "module.runtime-registration-manifest.json"
    )
    assert case["translation_unit_registration_order_ordinal"] == 1
    assert case["descriptor_total"] >= 0
    probe_payload = case["probe_payload"]
    assert probe_payload["success_status"] == 0
    assert probe_payload["duplicate_status"] == -2
    assert probe_payload["out_of_order_status"] == -3
    assert probe_payload["invalid_status"] == -1
