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
    / "check_m251_b002_metadata_completeness_and_duplicate_suppression_semantics.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m251_b002_metadata_completeness_and_duplicate_suppression_semantics",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m251_b002_metadata_completeness_and_duplicate_suppression_semantics.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m251-b002-runtime-export-enforcement-contract-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m251_b002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m251/M251-B002/")


def test_contract_fails_closed_when_expectations_drop_forward_protocol_rule(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m251_metadata_completeness_and_duplicate_suppression_semantics_core_feature_implementation_b002_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Forward `@protocol` declarations remain legal dependency hints rather than\n   incomplete export candidates.",
            "Forward protocol declarations need follow-up.",
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
    assert any(failure["check_id"] == "M251-B002-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_pipeline_drops_o3s260_gate(tmp_path: Path) -> None:
    drift_pipeline = tmp_path / "objc3_frontend_pipeline.cpp"
    drift_pipeline.write_text(
        contract.DEFAULT_FRONTEND_PIPELINE.read_text(encoding="utf-8").replace(
            '"O3S260"',
            '"O3S261"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--frontend-pipeline",
            str(drift_pipeline),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M251-B002-PIPE-05" for failure in payload["failures"])


def test_dynamic_probes_cover_happy_and_fail_closed_runtime_export_paths(tmp_path: Path) -> None:
    if not contract.DEFAULT_RUNNER_EXE.exists() or not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native objc3c binaries are not built")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 5

    class_case = next(case for case in payload["dynamic_cases"] if case["case_id"] == "M251-B002-CASE-CLASS")
    ir_case = next(case for case in payload["dynamic_cases"] if case["case_id"] == "M251-B002-CASE-IR")
    duplicate_case = next(case for case in payload["dynamic_cases"] if case["case_id"] == "M251-B002-CASE-DUPLICATE")
    incomplete_case = next(case for case in payload["dynamic_cases"] if case["case_id"] == "M251-B002-CASE-INCOMPLETE")
    illegal_case = next(case for case in payload["dynamic_cases"] if case["case_id"] == "M251-B002-CASE-ILLEGAL")

    assert class_case["status"] == 0
    assert class_case["success"] is True
    assert class_case["emit_stage"]["attempted"] is False
    assert class_case["emit_stage"]["skipped"] is True
    assert class_case["runtime_export_enforcement_contract_id"] == "objc3c-runtime-export-enforcement/m251-b002-v1"
    assert class_case["runtime_export_ready_for_runtime_export"] is True
    assert class_case["runtime_export_duplicate_runtime_identity_sites"] == 0
    assert class_case["runtime_export_incomplete_declaration_sites"] == 0
    assert class_case["runtime_export_illegal_redeclaration_mix_sites"] == 0
    assert class_case["runtime_export_metadata_shape_drift_sites"] == 0
    assert class_case["runtime_export_enforcement_failure_reason"] == ""

    assert ir_case["process_exit_code"] == 0
    assert ir_case["ir_contains_contract_comment"] is True
    assert ir_case["ir_contains_named_metadata"] is True

    assert duplicate_case["process_exit_code"] != 0
    assert "O3S200" in duplicate_case["diagnostic_codes"]

    assert incomplete_case["process_exit_code"] != 0
    assert "O3S260" in incomplete_case["diagnostic_codes"]

    assert illegal_case["process_exit_code"] != 0
    assert "O3S206" in illegal_case["diagnostic_codes"]
