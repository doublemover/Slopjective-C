from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m252_e002_conformance_corpus_and_docs_sync_for_metadata_graph_closure.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m252_e002_conformance_corpus_and_docs_sync_for_metadata_graph_closure",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m252_e002_conformance_corpus_and_docs_sync_for_metadata_graph_closure.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m252-e002-conformance-corpus-and-docs-sync-for-metadata-graph-closure-v1"
    assert payload["contract_id"] == "objc3c-metadata-graph-closure-conformance-corpus-doc-sync/m252-e002-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 60
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["dynamic_probes_executed"] is True
    assert payload["positive_case_count"] == 4
    assert payload["negative_case_count"] == 2
    assert payload["real_integrated_path"] is True
    assert len(payload["corpus_cases"]) == 6
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m252_e002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m252/M252-E002/")


def test_contract_fails_closed_when_expectations_drop_runner_script(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m252_e002_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`scripts/run_m252_e002_lane_e_readiness.py`",
            "`scripts/run_missing_lane_e_readiness.py`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M252-E002-DOC-EXP-06" for failure in payload["failures"])


def test_contract_fails_closed_when_runner_script_drops_d002_step(tmp_path: Path) -> None:
    drift_script = tmp_path / "run_m252_e002_lane_e_readiness.py"
    drift_script.write_text(
        contract.DEFAULT_RUNNER_SCRIPT.read_text(encoding="utf-8").replace(
            '"check:objc3c:m252-d002-artifact-packaging-and-binary-boundary-for-metadata-payloads"',
            '"check:objc3c:m252-d099-missing"',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--runner-script", str(drift_script), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M252-E002-RUN-04" for failure in payload["failures"])


def test_contract_fails_closed_when_e001_summary_loses_readiness(tmp_path: Path) -> None:
    drift_summary = tmp_path / "m252_e001_summary.json"
    payload = json.loads(contract.DEFAULT_E001_SUMMARY.read_text(encoding="utf-8"))
    payload["aggregate_semantic_closure_ready_for_section_emission"] = False
    drift_summary.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--e001-summary", str(drift_summary), "--summary-out", str(summary_out)])

    assert exit_code == 1
    contract_payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert contract_payload["ok"] is False
    assert any(failure["check_id"] == "M252-E002-E001-READY" for failure in contract_payload["failures"])


def test_contract_fails_closed_when_negative_fixture_loses_o3s206(tmp_path: Path) -> None:
    drift_fixture = tmp_path / "m252_b004_missing_interface_property.objc3"
    drift_fixture.write_text(
        contract.DEFAULT_CLASS_SYNTHESIS_FIXTURE.read_text(encoding="utf-8"),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--missing-interface-fixture", str(drift_fixture), "--summary-out", str(summary_out)])

    assert exit_code == 1
    contract_payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert contract_payload["ok"] is False
    assert any(failure["check_id"] == "M252-E002-CASE-MISSING-O3S206" for failure in contract_payload["failures"])
