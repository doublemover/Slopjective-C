from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT / "scripts" / "check_m251_a002_class_protocol_category_property_ivar_source_record_extraction_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m251_a002_class_protocol_category_property_ivar_source_record_extraction_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m251_a002_class_protocol_category_property_ivar_source_record_extraction_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-runner-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert (
        payload["mode"]
        == "m251-a002-class-protocol-category-property-ivar-source-record-extraction-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["runner_probes_executed"] is False
    assert payload["checks_total"] >= 45
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m251_a002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m251/M251-A002/")


def test_contract_fails_closed_when_expectations_drop_source_record_set_reference(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m251_class_protocol_category_property_ivar_source_record_extraction_core_feature_implementation_a002_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`Objc3RuntimeMetadataSourceRecordSet`",
            "runtime metadata source record set",
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
    assert any(failure["check_id"] == "M251-A002-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_frontend_artifacts_drop_runtime_metadata_source_records_block(
    tmp_path: Path,
) -> None:
    drift_artifacts = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_artifacts.write_text(
        contract.DEFAULT_FRONTEND_ARTIFACTS.read_text(encoding="utf-8").replace(
            'manifest << "  \\\"runtime_metadata_source_records\\\": {\\n";',
            'manifest << "  \\\"runtime_metadata_records\\\": {\\n";',
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
    assert any(failure["check_id"] == "M251-A002-ART-05" for failure in payload["failures"])


def test_dynamic_runner_probes_accept_frontend_source_record_fixtures(tmp_path: Path) -> None:
    if not contract.DEFAULT_RUNNER_EXE.exists():
        pytest.skip("native frontend C API runner binary is not built")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["runner_probes_executed"] is True
    assert len(payload["runner_cases"]) == 2
    for case in payload["runner_cases"]:
        assert case["semantic_skipped"] is False
        assert case["stages"]["parse"]["diagnostics_errors"] == 0
        assert case["stages"]["sema"]["diagnostics_errors"] == 0
        assert case["stages"]["lower"]["diagnostics_errors"] == 0
        if not case["success"]:
            assert case["stages"]["emit"]["diagnostics_errors"] >= 1
            assert "O3L300" in case["last_error"] or "semantic parity surface is not ready" in case["last_error"]
