from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_c001_parse_lowering_readiness_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_c001_parse_lowering_readiness_contract",
    SCRIPT_PATH,
)
assert SPEC is not None and SPEC.loader is not None
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m226-c001-parse-lowering-readiness-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 60
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m226() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/reports/m226/")


def test_contract_fails_closed_when_pipeline_drops_diagnostics_transport(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    pipeline_drift = tmp_path / "objc3_frontend_pipeline.cpp"
    pipeline_drift.write_text(
        contract.ARTIFACTS["pipeline_source"]
        .read_text(encoding="utf-8")
        .replace(
            "TransportObjc3DiagnosticsToParsedProgram(result.stage_diagnostics, result.program);",
            "// drift: diagnostics transport removed",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["pipeline_source"] = pipeline_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-C001-PIP-06" for failure in payload["failures"])


def test_contract_fails_closed_when_lowering_boundary_drops_selector_global_ordering(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    lowering_drift = tmp_path / "objc3_lowering_contract.cpp"
    lowering_drift.write_text(
        contract.ARTIFACTS["lowering_contract_source"]
        .read_text(encoding="utf-8")
        .replace(
            "boundary.selector_global_ordering = kObjc3SelectorGlobalOrdering;",
            "boundary.selector_global_ordering = \"\";",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["lowering_contract_source"] = lowering_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-C001-LOW-04" for failure in payload["failures"])
