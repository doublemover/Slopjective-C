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
    / "check_m251_b003_illegal_runtime_exposed_declaration_diagnostics.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m251_b003_illegal_runtime_exposed_declaration_diagnostics",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m251_b003_illegal_runtime_exposed_declaration_diagnostics.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m251-b003-runtime-export-diagnostics-contract-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m251_b003() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m251/M251-B003/")


def test_contract_fails_closed_when_expectations_drop_precise_interface_rule(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m251_illegal_runtime_exposed_declaration_diagnostics_core_feature_expansion_b003_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Interface-only runtime export units report a precise reason naming the class\n   interface that is missing a matching `@implementation`.",
            "Interface-only runtime export units still fail closed.",
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
    assert any(failure["check_id"] == "M251-B003-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_pipeline_drops_category_precision(tmp_path: Path) -> None:
    drift_pipeline = tmp_path / "objc3_frontend_pipeline.cpp"
    drift_pipeline.write_text(
        contract.DEFAULT_FRONTEND_PIPELINE.read_text(encoding="utf-8").replace(
            "category '",
            "category-missing ",
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
    assert any(failure["check_id"] == "M251-B003-PIPE-04" for failure in payload["failures"])


def test_dynamic_probes_cover_happy_and_precise_fail_closed_runtime_export_paths(
    tmp_path: Path,
) -> None:
    if not contract.DEFAULT_RUNNER_EXE.exists() or not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native objc3c binaries are not built")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 3

    class_case = next(case for case in payload["dynamic_cases"] if case["case_id"] == "M251-B003-CASE-CLASS")
    incomplete_case = next(
        case for case in payload["dynamic_cases"] if case["case_id"] == "M251-B003-CASE-INCOMPLETE-INTERFACE"
    )
    category_case = next(
        case
        for case in payload["dynamic_cases"]
        if case["case_id"] == "M251-B003-CASE-CATEGORY-MISSING-IMPLEMENTATION"
    )

    assert class_case["process_exit_code"] == 0
    assert class_case["status"] == 0
    assert class_case["success"] is True
    assert class_case["runtime_export_ready_for_runtime_export"] is True

    assert incomplete_case["process_exit_code"] != 0
    assert "O3S260" in incomplete_case["diagnostic_codes"]
    assert any(
        "interface 'Widget' is missing a matching @implementation" in message
        for message in incomplete_case["diagnostic_messages"]
    )
    assert 4 in incomplete_case["diagnostic_lines"]

    assert category_case["process_exit_code"] != 0
    assert "O3S260" in category_case["diagnostic_codes"]
    assert any(
        "category 'Widget(Tracing)' is missing a matching @implementation" in message
        for message in category_case["diagnostic_messages"]
    )
    assert 4 in category_case["diagnostic_lines"]
