from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m252_a003_protocol_category_property_ivar_export_graph_completion.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m252_a003_protocol_category_property_ivar_export_graph_completion",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m252_a003_protocol_category_property_ivar_export_graph_completion.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-runner-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m252-a003-protocol-category-property-ivar-export-graph-completion-v1"
    assert payload["ok"] is True
    assert payload["runner_probes_executed"] is False
    assert payload["checks_total"] >= 50
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m252_a003() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m252/M252-A003/")


def test_contract_fails_closed_when_expectations_drop_category_node_entries_reference(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path / "m252_protocol_category_property_ivar_export_graph_completion_a003_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`category_node_entries`",
            "category node entries",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--expectations-doc",
            str(drift_doc),
            "--skip-runner-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M252-A003-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_frontend_artifacts_drop_property_node_entries_block(tmp_path: Path) -> None:
    drift_artifacts = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_artifacts.write_text(
        contract.DEFAULT_FRONTEND_ARTIFACTS.read_text(encoding="utf-8").replace(
            '\\"property_node_entries\\":[',
            '\\"property_nodes_entries\\":[',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--frontend-artifacts",
            str(drift_artifacts),
            "--skip-runner-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M252-A003-ART-03" for failure in payload["failures"])


def test_dynamic_runner_probe_accepts_both_fixtures(tmp_path: Path) -> None:
    if not contract.DEFAULT_RUNNER_EXE.exists():
        pytest.skip("native frontend C API runner binary is not built")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["runner_probes_executed"] is True

    class_case = payload["runner_cases"]["class_fixture"]
    class_graph = class_case["graph"]
    assert class_graph["source_graph_complete"] is True
    assert class_graph["protocol_nodes"] == 2
    assert class_graph["category_nodes"] == 0
    assert len(class_graph["property_node_entries"]) == 3
    assert len(class_graph["method_node_entries"]) == 5
    assert len(class_graph["ivar_node_entries"]) == 1

    category_case = payload["runner_cases"]["category_fixture"]
    category_graph = category_case["graph"]
    assert category_graph["source_graph_complete"] is True
    assert category_graph["category_nodes"] == 1
    assert len(category_graph["category_node_entries"]) == 1
    assert len(category_graph["property_node_entries"]) == 3
    assert len(category_graph["method_node_entries"]) == 3
    assert len(category_graph["ivar_node_entries"]) == 1
