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
    / "check_m251_c003_metadata_object_inspection_harness_conformance_matrix_implementation_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m251_c003_metadata_object_inspection_harness_conformance_matrix_implementation_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m251_c003_metadata_object_inspection_harness_conformance_matrix_implementation_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m251-c003-runtime-metadata-object-inspection-harness-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 25
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m251_c003() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m251/M251-C003/")


def test_contract_fails_closed_when_expectations_drop_objdump_command(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m251_metadata_object_inspection_harness_conformance_matrix_implementation_c003_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`llvm-objdump --syms module.obj`",
            "`objdump --syms module.obj`",
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
    assert any(failure["check_id"] == "M251-C003-DOC-EXP-05" for failure in payload["failures"])


def test_contract_fails_closed_when_frontend_artifacts_drop_object_inspection_contract_key(
    tmp_path: Path,
) -> None:
    drift_artifacts = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_artifacts.write_text(
        contract.DEFAULT_FRONTEND_ARTIFACTS.read_text(encoding="utf-8").replace(
            "runtime_metadata_object_inspection_contract_id",
            "runtime_metadata_object_inspection_key",
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
    assert any(failure["check_id"] == "M251-C003-ART-03" for failure in payload["failures"])


def test_dynamic_probes_cover_manifest_and_object_inspection_surfaces(tmp_path: Path) -> None:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native objc3c binary is not built")
    if contract.resolve_tool("llvm-readobj.exe", None) is None:
        pytest.skip("llvm-readobj.exe is not available")
    if contract.resolve_tool("llvm-objdump.exe", None) is None:
        pytest.skip("llvm-objdump.exe is not available")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 2

    manifest_case = next(
        case
        for case in payload["dynamic_cases"]
        if case["case_id"] == "M251-C003-CASE-MANIFEST-ZERO-DESCRIPTOR"
    )
    object_case = next(
        case
        for case in payload["dynamic_cases"]
        if case["case_id"] == "M251-C003-CASE-OBJECT-ZERO-DESCRIPTOR"
    )

    assert manifest_case["process_exit_code"] == 0
    assert manifest_case["status"] == 0
    assert manifest_case["success"] is True
    assert manifest_case["runtime_metadata_object_inspection_contract_id"] == contract.CONTRACT_ID
    assert (
        manifest_case["runtime_metadata_object_inspection_scaffold_contract_id"]
        == "objc3c-runtime-metadata-section-scaffold/m251-c002-v1"
    )
    assert manifest_case["runtime_metadata_object_inspection_matrix_published"] is True
    assert manifest_case["runtime_metadata_object_inspection_fail_closed"] is True
    assert manifest_case["runtime_metadata_object_inspection_uses_llvm_readobj"] is True
    assert manifest_case["runtime_metadata_object_inspection_uses_llvm_objdump"] is True
    assert manifest_case["runtime_metadata_object_inspection_matrix_row_count"] == 2
    assert manifest_case["runtime_metadata_object_inspection_fixture_path"].endswith(
        "m251_runtime_metadata_object_inspection_zero_descriptor.objc3"
    )
    assert manifest_case["runtime_metadata_object_inspection_section_inventory_command"] == (
        "llvm-readobj --sections module.obj"
    )
    assert manifest_case["runtime_metadata_object_inspection_symbol_inventory_command"] == (
        "llvm-objdump --syms module.obj"
    )
    assert manifest_case["object_backend"] in {"llvm-direct", "llc"}

    assert object_case["compile_exit_code"] == 0
    assert object_case["status"] == 0
    assert object_case["success"] is True
    assert object_case["object_exists"] is True
    assert object_case["object_backend"] in {"llvm-direct", "llc"}
    assert sorted(section["name"] for section in object_case["metadata_sections"]) == sorted(
        contract.EXPECTED_SECTIONS
    )
    assert all(section["raw_data_size"] == 8 for section in object_case["metadata_sections"])
    assert sorted(object_case["retained_symbols"]) == sorted(contract.EXPECTED_SYMBOLS)
