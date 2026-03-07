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
    / "check_m251_b001_object_model_abi_invariants_and_legality_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m251_b001_object_model_abi_invariants_and_legality_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m251_b001_object_model_abi_invariants_and_legality_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m251-b001-runtime-export-legality-contract-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m251_b001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m251/M251-B001/")


def test_contract_fails_closed_when_expectations_drop_pending_enforcement_reference(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m251_object_model_abi_invariants_and_legality_contract_and_architecture_freeze_b001_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "does not yet reject duplicate runtime identities",
            "duplicate identity work is future work",
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
    assert any(failure["check_id"] == "M251-B001-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_frontend_types_drop_legality_boundary_struct(tmp_path: Path) -> None:
    drift_types = tmp_path / "objc3_frontend_types.h"
    drift_types.write_text(
        contract.DEFAULT_FRONTEND_TYPES.read_text(encoding="utf-8").replace(
            "struct Objc3RuntimeExportLegalityBoundary {",
            "struct Objc3RuntimeExportFreezeBoundary {",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--frontend-types",
            str(drift_types),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M251-B001-TYP-02" for failure in payload["failures"])


def test_dynamic_probes_cover_manifest_and_ir_legality_artifacts(tmp_path: Path) -> None:
    if not contract.DEFAULT_RUNNER_EXE.exists() or not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native objc3c binaries are not built")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 3

    class_case = next(case for case in payload["dynamic_cases"] if case["case_id"] == "M251-B001-CASE-CLASS")
    category_case = next(case for case in payload["dynamic_cases"] if case["case_id"] == "M251-B001-CASE-CATEGORY")
    ir_case = next(case for case in payload["dynamic_cases"] if case["case_id"] == "M251-B001-CASE-IR")

    assert class_case["status"] == 0
    assert class_case["success"] is True
    assert class_case["emit_stage"]["attempted"] is False
    assert class_case["emit_stage"]["skipped"] is True
    assert class_case["runtime_export_semantic_boundary_frozen"] is True
    assert class_case["runtime_export_fail_closed"] is True
    assert class_case["runtime_export_boundary_ready"] is True

    assert category_case["status"] == 0
    assert category_case["success"] is True
    assert category_case["runtime_export_category_record_count"] > 0

    assert ir_case["process_exit_code"] == 0
    assert ir_case["ir_contains_contract_comment"] is True
    assert ir_case["ir_contains_named_metadata"] is True
