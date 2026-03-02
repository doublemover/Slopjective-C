from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m227_a004_semantic_pass_core_feature_expansion_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m227_a004_semantic_pass_core_feature_expansion_contract",
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
    assert payload["mode"] == "m227-sema-pass-core-feature-expansion-contract-a004-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 10
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m227_semantic_pass_core_feature_expansion_expectations.md"
    drift_doc.write_text(
        (
            ROOT / "docs" / "contracts" / "m227_semantic_pass_core_feature_expansion_expectations.md"
        ).read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-sema-pass-core-feature-expansion/m227-a004-v1`",
            "Contract ID: `objc3c-sema-pass-core-feature-expansion/m227-a004-drift`",
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
    assert any(
        failure["check_id"] == "M227-A004-DOC-01"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_artifact_emission_stringifies_numeric_field(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_artifacts = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_artifacts.write_text(
        (
            ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
        ).read_text(encoding="utf-8")
        + "\n\\\"pass_flow_diagnostics_emitted_by_build\\\":\\\"\n",
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["frontend_artifacts"] = drift_artifacts
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M227-A004-ART-07"
        for failure in payload["failures"]
    )
