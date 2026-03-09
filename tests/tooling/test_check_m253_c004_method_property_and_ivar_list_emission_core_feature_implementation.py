from __future__ import annotations

import importlib.util
import json
import subprocess
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m253_c004_method_property_and_ivar_list_emission_core_feature_implementation.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m253_c004_method_property_and_ivar_list_emission_core_feature_implementation",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m253_c004_method_property_and_ivar_list_emission_core_feature_implementation.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m253-c004-method-property-and-ivar-list-emission-core-feature-implementation-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 45
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m253_c004() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m253/M253-C004/")


def test_contract_fails_closed_when_expectations_drop_method_payload_model(tmp_path: Path) -> None:
    drift_doc = tmp_path / "expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`owner-scoped-method-table-globals-with-inline-entry-records`",
            "`method-tables`",
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
    assert any(failure["check_id"] == "M253-C004-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_ir_emitter_drops_member_table_named_metadata(tmp_path: Path) -> None:
    drift_ir = tmp_path / "objc3_ir_emitter.cpp"
    drift_ir.write_text(
        contract.DEFAULT_IR_EMITTER.read_text(encoding="utf-8").replace(
            'out << "!objc3.objc_runtime_member_table_emission = !{!58}\\n";',
            'out << "!objc3.objc_runtime_member_tables = !{!58}\\n";',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--ir-emitter",
            str(drift_ir),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M253-C004-IR-01" for failure in payload["failures"])


def test_dynamic_probe_records_real_member_table_payloads(tmp_path: Path) -> None:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native objc3c binary is not built")

    subprocess.run(
        ["pwsh", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", "scripts/build_objc3c_native.ps1"],
        cwd=ROOT,
        check=True,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 2

    cases = {case["case_id"]: case for case in payload["dynamic_cases"]}
    class_case = cases["M253-C004-CASE-CLASS-PROTOCOL-PROPERTY-IVAR"]
    assert class_case["process_exit_code"] == 0
    assert class_case["backend"] == "llvm-direct"
    assert class_case["diagnostics_is_empty"] is True
    assert class_case["method_list_bundle_count"] == 5
    assert class_case["method_entry_count"] == 7
    assert class_case["property_descriptor_count"] == 5
    assert class_case["ivar_descriptor_count"] == 2
    assert class_case["objc3.runtime.class_descriptors_raw_size"] is not None and class_case["objc3.runtime.class_descriptors_raw_size"] >= 256
    assert class_case["objc3.runtime.property_descriptors_raw_size"] is not None and class_case["objc3.runtime.property_descriptors_raw_size"] >= 256
    assert contract.BOUNDARY_COMMENT_PREFIX in class_case["boundary_line"]

    category_case = cases["M253-C004-CASE-CATEGORY-PROTOCOL-PROPERTY"]
    assert category_case["process_exit_code"] == 0
    assert category_case["backend"] == "llvm-direct"
    assert category_case["diagnostics_is_empty"] is True
    assert category_case["method_list_bundle_count"] == 3
    assert category_case["method_entry_count"] == 5
    assert category_case["property_descriptor_count"] == 5
    assert category_case["ivar_descriptor_count"] == 2
    assert category_case["objc3.runtime.category_descriptors_raw_size"] is not None and category_case["objc3.runtime.category_descriptors_raw_size"] >= 256
    assert category_case["objc3.runtime.property_descriptors_raw_size"] is not None and category_case["objc3.runtime.property_descriptors_raw_size"] >= 256
    assert contract.BOUNDARY_COMMENT_PREFIX in category_case["boundary_line"]
