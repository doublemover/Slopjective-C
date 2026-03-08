from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m252_b003_category_attachment_duplication_ambiguity_diagnostics.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m252_b003_category_attachment_duplication_ambiguity_diagnostics",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m252_b003_category_attachment_duplication_ambiguity_diagnostics.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-runner-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m252-b003-category-attachment-duplication-ambiguity-diagnostics-v1"
    assert payload["ok"] is True
    assert payload["runner_probes_executed"] is False
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m252_b003() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m252/M252-B003/")


def test_contract_fails_closed_when_expectations_drop_o3s262_anchor(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m252_category_attachment_duplication_ambiguity_diagnostics_b003_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`O3S262`",
            "`duplicate-runtime-member`",
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
    assert any(failure["check_id"] == "M252-B003-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_pipeline_drops_o3s263_anchor(tmp_path: Path) -> None:
    drift_pipeline = tmp_path / "objc3_frontend_pipeline.cpp"
    drift_pipeline.write_text(
        contract.DEFAULT_FRONTEND_PIPELINE.read_text(encoding="utf-8").replace(
            '"O3S263"',
            '"O3S26X"',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run([
        "--frontend-pipeline",
        str(drift_pipeline),
        "--skip-runner-probes",
        "--summary-out",
        str(summary_out),
    ])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M252-B003-PIPE-04" for failure in payload["failures"])


def test_dynamic_runner_probes_cover_valid_collision_and_ambiguity_cases(tmp_path: Path) -> None:
    if not contract.DEFAULT_RUNNER_EXE.exists():
        pytest.skip("native frontend C API runner binary is not built")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["runner_probes_executed"] is True

    runner_cases = payload["runner_cases"]
    assert runner_cases["valid_category_attachment"]["observed_codes"] == []
    assert runner_cases["category_attachment_collision"]["observed_codes"] == ["O3S261", "O3S263"]
    assert runner_cases["duplicate_runtime_member_ambiguity"]["observed_codes"] == ["O3S261", "O3S263", "O3S262"]
