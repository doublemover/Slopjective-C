from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m252_b001_executable_metadata_semantic_consistency_freeze.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m252_b001_executable_metadata_semantic_consistency_freeze",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m252_b001_executable_metadata_semantic_consistency_freeze.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-runner-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m252-b001-executable-metadata-semantic-consistency-freeze-v1"
    assert payload["ok"] is True
    assert payload["runner_probes_executed"] is False
    assert payload["checks_total"] >= 35
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m252_b001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m252/M252-B001/")


def test_contract_fails_closed_when_expectations_drop_lowering_pending_reference(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m252_executable_metadata_semantic_consistency_freeze_b001_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "does not yet admit lowering",
            "lowering is future work",
            1,
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
    assert any(failure["check_id"] == "M252-B001-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_frontend_types_drop_boundary_struct(tmp_path: Path) -> None:
    drift_types = tmp_path / "objc3_frontend_types.h"
    drift_types.write_text(
        contract.DEFAULT_FRONTEND_TYPES.read_text(encoding="utf-8").replace(
            "struct Objc3ExecutableMetadataSemanticConsistencyBoundary {",
            "struct Objc3ExecutableMetadataSemanticBoundary {",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run([
        "--frontend-types",
        str(drift_types),
        "--skip-runner-probes",
        "--summary-out",
        str(summary_out),
    ])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M252-B001-TYP-02" for failure in payload["failures"])


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
    class_boundary = class_case["boundary"]
    assert class_boundary["semantic_boundary_frozen"] is True
    assert class_boundary["fail_closed"] is True
    assert class_boundary["ready"] is True
    assert class_boundary["lowering_admission_ready"] is False
    assert class_boundary["category_node_count"] == 0

    category_case = payload["runner_cases"]["category_fixture"]
    category_boundary = category_case["boundary"]
    assert category_boundary["semantic_boundary_frozen"] is True
    assert category_boundary["fail_closed"] is True
    assert category_boundary["ready"] is True
    assert category_boundary["category_node_count"] == 1
