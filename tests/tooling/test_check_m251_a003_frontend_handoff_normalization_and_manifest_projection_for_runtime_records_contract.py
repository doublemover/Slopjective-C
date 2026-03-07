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
    / "check_m251_a003_frontend_handoff_normalization_and_manifest_projection_for_runtime_records_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m251_a003_frontend_handoff_normalization_and_manifest_projection_for_runtime_records_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m251_a003_frontend_handoff_normalization_and_manifest_projection_for_runtime_records_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)



def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m251-a003-runtime-record-manifest-handoff-contract-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 35
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []



def test_contract_default_summary_out_is_under_tmp_reports_m251_a003() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m251/M251-A003/")



def test_contract_fails_closed_when_expectations_drop_manifest_only_success_reference(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m251_frontend_handoff_normalization_and_manifest_projection_for_runtime_records_core_feature_expansion_a003_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object` succeeds",
            "manifest-only C API runs now",
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
    assert any(failure["check_id"] == "M251-A003-DOC-EXP-03" for failure in payload["failures"])



def test_contract_fails_closed_when_frontend_anchor_drops_manifest_write_on_available_payload(
    tmp_path: Path,
) -> None:
    drift_anchor = tmp_path / "frontend_anchor.cpp"
    drift_anchor.write_text(
        contract.DEFAULT_FRONTEND_ANCHOR.read_text(encoding="utf-8").replace(
            "if (options->emit_manifest != 0 && has_out_dir && !product.artifact_bundle.manifest_json.empty()) {",
            "if (result->status == OBJC3C_FRONTEND_STATUS_OK && options->emit_manifest != 0 && has_out_dir) {",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--frontend-anchor",
            str(drift_anchor),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M251-A003-ANCHOR-04" for failure in payload["failures"])



def test_dynamic_probes_cover_manifest_only_success_and_cli_manifest_preservation(tmp_path: Path) -> None:
    if not contract.DEFAULT_RUNNER_EXE.exists() or not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native objc3c binaries are not built")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 3

    runner_cases = [case for case in payload["dynamic_cases"] if case["case_id"] != "M251-A003-CASE-CLI"]
    cli_case = next(case for case in payload["dynamic_cases"] if case["case_id"] == "M251-A003-CASE-CLI")

    for case in runner_cases:
        assert case["status"] == 0
        assert case["success"] is True
        assert case["emit_stage"]["attempted"] is False
        assert case["emit_stage"]["skipped"] is True
        manifest_path = ROOT / case["manifest_path"]
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        assert "runtime_metadata_source_records" in manifest

    assert cli_case["process_exit_code"] != 0
    assert cli_case["manifest_exists"] is True
    assert cli_case["ir_exists"] is False
    assert cli_case["object_exists"] is False
