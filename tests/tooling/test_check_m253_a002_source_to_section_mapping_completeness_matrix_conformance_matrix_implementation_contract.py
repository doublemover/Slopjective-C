from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m253_a002_source_to_section_mapping_completeness_matrix_conformance_matrix_implementation_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m253_a002_source_to_section_mapping_completeness_matrix_conformance_matrix_implementation_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m253_a002_source_to_section_mapping_completeness_matrix_conformance_matrix_implementation_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_without_dynamic_probes(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m253-a002-source-to-section-mapping-completeness-matrix-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["expected_row_count"] == 9
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m253_a002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m253/M253-A002/")


def test_contract_fails_closed_when_expectations_drop_surface_path(tmp_path: Path) -> None:
    drift_doc = tmp_path / "expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`frontend.pipeline.semantic_surface.objc_runtime_metadata_source_to_section_matrix`",
            "`frontend.pipeline.semantic_surface.objc_runtime_metadata_source_to_section_inventory`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M253-A002-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_frontend_artifacts_drop_matrix_surface(tmp_path: Path) -> None:
    drift_cpp = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_cpp.write_text(
        contract.DEFAULT_FRONTEND_ARTIFACTS.read_text(encoding="utf-8").replace(
            "objc_runtime_metadata_source_to_section_matrix",
            "objc_runtime_metadata_source_to_section_inventory",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--frontend-artifacts", str(drift_cpp), "--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M253-A002-ART-03" for failure in payload["failures"])


@pytest.mark.skipif(not contract.DEFAULT_RUNNER_EXE.exists(), reason="frontend runner binary is not available")
def test_contract_dynamic_probe_passes_when_tooling_is_available(tmp_path: Path) -> None:
    if contract.resolve_tool("llvm-readobj.exe", None) is None:
        pytest.skip("llvm-readobj.exe is not available")
    if contract.resolve_tool("llvm-objdump.exe", None) is None:
        pytest.skip("llvm-objdump.exe is not available")
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("objc3c-native.exe is not available")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    inspection_case = payload["dynamic_cases"]["inspection_fixture"]
    matrix = inspection_case["matrix"]
    assert matrix["contract_id"] == contract.CONTRACT_ID
    assert matrix["matrix_row_count"] == 9
    assert matrix["row_ordering_model"] == contract.ROW_ORDERING_MODEL
    assert any(row["row_key"] == "class-node-to-emission" for row in matrix["rows"])
    assert any(row["row_key"] == "method-node-to-emission" for row in matrix["rows"])
    assert inspection_case["object_backend"] in {"llvm-direct", "llc"}
    section_names = sorted(section["name"] for section in inspection_case["metadata_sections"])
    assert section_names == sorted(contract.EXPECTED_INSPECTION_SECTIONS)
