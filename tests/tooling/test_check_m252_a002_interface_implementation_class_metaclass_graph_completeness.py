from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m252_a002_interface_implementation_class_metaclass_graph_completeness.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m252_a002_interface_implementation_class_metaclass_graph_completeness",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m252_a002_interface_implementation_class_metaclass_graph_completeness.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-runner-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m252-a002-interface-implementation-class-metaclass-graph-completeness-v1"
    assert payload["ok"] is True
    assert payload["runner_probes_executed"] is False
    assert payload["checks_total"] >= 55
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m252_a002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m252/M252-A002/")


def test_contract_fails_closed_when_expectations_drop_owner_edges_reference(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m252_interface_implementation_class_metaclass_graph_completeness_a002_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`owner_edges`",
            "owner edges",
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
    assert any(failure["check_id"] == "M252-A002-DOC-EXP-05" for failure in payload["failures"])


def test_contract_fails_closed_when_frontend_artifacts_drop_owner_edges_block(tmp_path: Path) -> None:
    drift_artifacts = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_artifacts.write_text(
        contract.DEFAULT_FRONTEND_ARTIFACTS.read_text(encoding="utf-8").replace(
            '\\"owner_edges\\":[',
            '\\"graph_edges\\":[',
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
    assert any(failure["check_id"] == "M252-A002-ART-04" for failure in payload["failures"])


def test_dynamic_runner_probe_accepts_graph_fixture(tmp_path: Path) -> None:
    if not contract.DEFAULT_RUNNER_EXE.exists():
        pytest.skip("native frontend C API runner binary is not built")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["runner_probes_executed"] is True
    runner_case = payload["runner_case"]
    assert runner_case["success"] is True
    graph = runner_case["graph"]
    assert graph["contract_id"] == "objc3c-executable-metadata-source-graph-completeness/m252-a002-v1"
    assert graph["source_graph_complete"] is True
    assert graph["ready_for_semantic_closure"] is True
    assert graph["ready_for_lowering"] is False
    assert len(graph["interface_node_entries"]) == 2
    assert len(graph["implementation_node_entries"]) == 2
    assert len(graph["class_node_entries"]) == 2
    assert len(graph["metaclass_node_entries"]) == 2
    assert len(graph["owner_edges"]) >= 8
