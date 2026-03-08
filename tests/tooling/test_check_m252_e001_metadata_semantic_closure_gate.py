from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m252_e001_metadata_semantic_closure_gate.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m252_e001_metadata_semantic_closure_gate",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_m252_e001_metadata_semantic_closure_gate.py")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m252-e001-metadata-semantic-closure-gate-v1"
    assert payload["contract_id"] == "objc3c-executable-metadata-semantic-closure-gate/m252-e001-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["aggregate_semantic_closure_ready_for_section_emission"] is True
    assert payload["source_graph_cases_ready"] == 2
    assert payload["diagnostic_negative_case_count"] == 2
    assert payload["debug_projection_row_count"] == 3
    assert payload["runtime_metadata_chunk_count"] == 3
    assert payload["next_implementation_issue"] == "M253-A001"
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m252_e001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m252/M252-E001/")


def test_contract_fails_closed_when_expectations_drop_d002_summary(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m252_e001_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`tmp/reports/m252/M252-D002/artifact_packaging_and_binary_boundary_for_metadata_payloads_summary.json`",
            "`tmp/reports/m252/M252-D099/missing.json`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M252-E001-DOC-EXP-06" for failure in payload["failures"])


def test_contract_fails_closed_when_package_lane_e_readiness_drops_d002(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            "npm run check:objc3c:m252-d002-lane-d-readiness && ",
            "",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M252-E001-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_a003_graph_completion_drifts(tmp_path: Path) -> None:
    drift_summary = tmp_path / "a003.json"
    payload = json.loads(contract.DEFAULT_A003_SUMMARY.read_text(encoding="utf-8"))
    payload["runner_cases"]["class_fixture"]["graph"]["source_graph_complete"] = False
    drift_summary.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--a003-summary", str(drift_summary), "--summary-out", str(summary_out)])

    assert exit_code == 1
    contract_payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert contract_payload["ok"] is False
    assert any(failure["check_id"] == "M252-E001-A003-CLASS-COMPLETE" for failure in contract_payload["failures"])


def test_contract_fails_closed_when_d002_chunk_order_drifts(tmp_path: Path) -> None:
    drift_summary = tmp_path / "d002.json"
    payload = json.loads(contract.DEFAULT_D002_SUMMARY.read_text(encoding="utf-8"))
    payload["dynamic_cases"][0]["chunk_names"] = ["typed_lowering_handoff", "runtime_ingest_packaging_contract", "debug_projection"]
    drift_summary.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--d002-summary", str(drift_summary), "--summary-out", str(summary_out)])

    assert exit_code == 1
    contract_payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert contract_payload["ok"] is False
    assert any(failure["check_id"] == "M252-E001-D002-CHUNKS" for failure in contract_payload["failures"])
