from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m243_c001_lowering_runtime_diagnostics_surfacing_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m243_c001_lowering_runtime_diagnostics_surfacing_contract",
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
    assert payload["mode"] == "m243-lowering-runtime-diagnostics-surfacing-freeze-contract-c001-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 24
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m243_lowering_runtime_diagnostics_surfacing_c001_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-freeze/m243-c001-v1`",
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-freeze/m243-c001-drift`",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["contract_doc"] = drift_doc
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-C001-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_diagnostics_artifact_drops_post_pipeline_merge(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_io = tmp_path / "objc3_diagnostics_artifacts.cpp"
    drift_io.write_text(
        contract.ARTIFACTS["diagnostics_artifacts_source"].read_text(encoding="utf-8").replace(
            "  diagnostics.insert(diagnostics.end(), post_pipeline_diagnostics.begin(), post_pipeline_diagnostics.end());",
            "  diagnostics.insert(diagnostics.end(), post_pipeline_diagnostics_removed.begin(), post_pipeline_diagnostics_removed.end());",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["diagnostics_artifacts_source"] = drift_io
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-C001-IO-02" for failure in payload["failures"])


def test_contract_fails_closed_when_c_api_emit_stage_drops_post_pipeline_seed(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_c_api = tmp_path / "frontend_anchor.cpp"
    drift_c_api.write_text(
        contract.ARTIFACTS["c_api_frontend_source"].read_text(encoding="utf-8").replace(
            "  std::vector<std::string> emit_diagnostics = product.artifact_bundle.post_pipeline_diagnostics;",
            "  std::vector<std::string> emit_diagnostics = product.artifact_bundle.stage_diagnostics.semantic;",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["c_api_frontend_source"] = drift_c_api
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-C001-CAPI-01" for failure in payload["failures"])
