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
    / "check_m251_c002_llvm_global_and_section_scaffold_for_runtime_metadata_payloads.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m251_c002_llvm_global_and_section_scaffold_for_runtime_metadata_payloads",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m251_c002_llvm_global_and_section_scaffold_for_runtime_metadata_payloads.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m251-c002-runtime-metadata-section-scaffold-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m251_c002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m251/M251-C002/")


def test_contract_fails_closed_when_expectations_drop_llvm_used_anchor(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m251_llvm_global_and_section_scaffold_for_runtime_metadata_payloads_c002_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "The scaffold is retained through `@llvm.used`",
            "The scaffold is retained through the metadata retention root",
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
    assert any(failure["check_id"] == "M251-C002-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_frontend_artifacts_drop_scaffold_contract_key(tmp_path: Path) -> None:
    drift_artifacts = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_artifacts.write_text(
        contract.DEFAULT_FRONTEND_ARTIFACTS.read_text(encoding="utf-8").replace(
            "runtime_metadata_section_scaffold_contract_id",
            "runtime_metadata_section_scaffold_key",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--frontend-artifacts",
            str(drift_artifacts),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M251-C002-ART-02" for failure in payload["failures"])


def test_dynamic_probes_cover_manifest_ir_and_object_scaffold_surfaces(tmp_path: Path) -> None:
    if not contract.DEFAULT_RUNNER_EXE.exists() or not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native objc3c binaries are not built")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 2

    manifest_case = next(case for case in payload["dynamic_cases"] if case["case_id"] == "M251-C002-CASE-MANIFEST-CLASS")
    native_case = next(case for case in payload["dynamic_cases"] if case["case_id"] == "M251-C002-CASE-NATIVE-HELLO")

    assert manifest_case["process_exit_code"] == 0
    assert manifest_case["status"] == 0
    assert manifest_case["success"] is True
    assert manifest_case["runtime_metadata_section_scaffold_contract_id"] == contract.CONTRACT_ID
    assert manifest_case["runtime_metadata_section_scaffold_emitted"] is True
    assert manifest_case["runtime_metadata_section_scaffold_uses_llvm_used"] is True
    assert manifest_case["runtime_metadata_section_scaffold_class_descriptor_count"] > 0
    assert manifest_case["runtime_metadata_section_scaffold_protocol_descriptor_count"] > 0
    assert manifest_case["runtime_metadata_section_scaffold_property_descriptor_count"] > 0
    assert manifest_case["runtime_metadata_section_scaffold_ivar_descriptor_count"] > 0
    assert manifest_case["runtime_metadata_section_scaffold_total_retained_global_count"] == manifest_case["runtime_metadata_section_scaffold_total_descriptor_count"] + 6
    assert manifest_case["runtime_metadata_section_scaffold_image_info_symbol"] == "__objc3_image_info"
    assert manifest_case["runtime_metadata_section_scaffold_class_aggregate_symbol"] == "__objc3_sec_class_descriptors"

    assert native_case["process_exit_code"] == 0
    assert native_case["ir_contains_contract_comment"] is True
    assert native_case["ir_contains_image_info_global"] is True
    assert native_case["ir_contains_zero_count_class_aggregate"] is True
    assert native_case["ir_contains_zero_count_protocol_aggregate"] is True
    assert native_case["ir_contains_zero_count_category_aggregate"] is True
    assert native_case["ir_contains_zero_count_property_aggregate"] is True
    assert native_case["ir_contains_zero_count_ivar_aggregate"] is True
    assert native_case["ir_contains_llvm_used"] is True
    assert native_case["ir_contains_named_metadata"] is True
    assert native_case["object_exists"] is True
