from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m252_b004_property_ivar_export_legality_synthesis_preconditions.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m252_b004_property_ivar_export_legality_synthesis_preconditions",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m252_b004_property_ivar_export_legality_synthesis_preconditions.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-runner-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m252-b004-property-ivar-export-legality-synthesis-preconditions-v1"
    assert payload["ok"] is True
    assert payload["runner_probes_executed"] is False
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m252_b004() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m252/M252-B004/")


def test_contract_fails_closed_when_expectations_drop_replay_key_anchor(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m252_property_ivar_export_legality_synthesis_preconditions_b004_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "lowering_property_synthesis_ivar_binding_replay_key",
            "property_synthesis_replay_key",
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
    assert any(failure["check_id"] == "M252-B004-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_frontend_artifacts_drop_sema_parity_handoff_anchor(tmp_path: Path) -> None:
    drift_artifacts = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_artifacts.write_text(
        contract.DEFAULT_FRONTEND_ARTIFACTS.read_text(encoding="utf-8").replace(
            "const Objc3SemaParityContractSurface &sema_parity_surface",
            "const Objc3FrontendPropertyAttributeSummary &summary",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run([
        "--frontend-artifacts",
        str(drift_artifacts),
        "--skip-runner-probes",
        "--summary-out",
        str(summary_out),
    ])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M252-B004-ART-02" for failure in payload["failures"])


def test_dynamic_runner_probes_cover_positive_and_negative_property_export_cases(tmp_path: Path) -> None:
    if not contract.DEFAULT_RUNNER_EXE.exists():
        pytest.skip("native frontend C API runner binary is not built")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["runner_probes_executed"] is True

    runner_cases = payload["runner_cases"]
    assert runner_cases["class_property_synthesis_ready"]["observed_replay_key"].startswith(
        "property_synthesis_sites=1;"
    )
    assert runner_cases["category_property_export_only"]["observed_replay_key"].startswith(
        "property_synthesis_sites=0;"
    )
    assert runner_cases["missing_interface_property"]["observed_codes"] == ["O3S206"]
    assert runner_cases["incompatible_property_signature"]["observed_codes"] == ["O3S206"]
