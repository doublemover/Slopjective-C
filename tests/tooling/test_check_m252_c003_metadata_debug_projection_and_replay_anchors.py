from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m252_c003_metadata_debug_projection_and_replay_anchors.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m252_c003_metadata_debug_projection_and_replay_anchors",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m252_c003_metadata_debug_projection_and_replay_anchors.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-runner-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m252-c003-metadata-debug-projection-and-replay-anchors-v1"
    assert payload["ok"] is True
    assert payload["runner_probes_executed"] is False
    assert payload["checks_total"] >= 50
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m252_c003() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m252/M252-C003/")


def test_contract_fails_closed_when_expectations_drop_contract_id_anchor(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m252_metadata_debug_projection_and_replay_anchors_c003_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "objc3c-executable-metadata-debug-projection/m252-c003-v1",
            "objc3c-executable-metadata-debug-projection/m252-c099-v1",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run([
        "--expectations-doc",
        str(drift_doc),
        "--skip-runner-probes",
        "--summary-out",
        str(summary_out),
    ])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M252-C003-DOC-EXP-02" for failure in payload["failures"])


def test_contract_fails_closed_when_ir_emitter_drops_named_metadata_anchor(tmp_path: Path) -> None:
    drift_ir = tmp_path / "objc3_ir_emitter.cpp"
    drift_ir.write_text(
        contract.DEFAULT_IR_EMITTER_CPP.read_text(encoding="utf-8").replace(
            "!objc3.objc_executable_metadata_debug_projection = !{!54}",
            "!objc3.objc_executable_metadata_projection = !{!54}",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run([
        "--ir-emitter-cpp",
        str(drift_ir),
        "--skip-runner-probes",
        "--summary-out",
        str(summary_out),
    ])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M252-C003-IRC-01" for failure in payload["failures"])


def test_dynamic_runner_probes_cover_manifest_and_ir_rows(tmp_path: Path) -> None:
    if not contract.DEFAULT_RUNNER_EXE.exists():
        pytest.skip("native frontend C API runner binary is not built")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["runner_probes_executed"] is True

    runner_cases = payload["runner_cases"]
    assert runner_cases["class_protocol_property_ivar"]["row_key"] == contract.CLASS_ROW_KEY
    assert runner_cases["category_protocol_property"]["row_key"] == contract.CATEGORY_ROW_KEY
    assert runner_cases["hello_ir_anchor"]["row_key"] == contract.IR_ROW_KEY
    assert runner_cases["class_protocol_property_ivar"]["active_typed_handoff_replay_key"]
    assert runner_cases["category_protocol_property"]["active_typed_handoff_replay_key"]
    assert runner_cases["hello_ir_anchor"]["debug_replay_key"]
