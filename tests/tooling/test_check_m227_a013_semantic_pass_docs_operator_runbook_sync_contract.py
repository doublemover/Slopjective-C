from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m227_a013_semantic_pass_docs_operator_runbook_sync_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m227_a013_semantic_pass_docs_operator_runbook_sync_contract",
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
    assert payload["mode"] == "m227-a013-semantic-pass-docs-operator-runbook-sync-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 30
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_runbook_loses_lane_anchor(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_runbook = tmp_path / "m227_wave_execution_runbook.md"
    drift_runbook.write_text(
        contract.ARTIFACTS["runbook"].read_text(encoding="utf-8").replace(
            "objc3c-runtime-facing-type-metadata-semantics-contract/m227-d001-v1",
            "objc3c-runtime-facing-type-metadata-semantics-contract/m227-d001-drift",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["runbook"] = drift_runbook
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-A013-RUN-05" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_loses_checker_anchor(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_packet = tmp_path / "m227_a013_semantic_pass_docs_operator_runbook_sync_packet.md"
    drift_packet.write_text(
        contract.ARTIFACTS["packet_doc"].read_text(encoding="utf-8").replace(
            "scripts/check_m227_a013_semantic_pass_docs_operator_runbook_sync_contract.py",
            "scripts/check_m227_a013_semantic_pass_docs_operator_runbook_sync_contract_drift.py",
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["packet_doc"] = drift_packet
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-A013-PKT-04" for failure in payload["failures"])


def test_contract_fails_closed_when_dependency_contract_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_dep = tmp_path / "m227_semantic_pass_cross_lane_integration_sync_expectations.md"
    drift_dep.write_text(
        contract.ARTIFACTS["a012_contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-semantic-pass-cross-lane-integration-sync/m227-a012-v1`",
            "Contract ID: `objc3c-semantic-pass-cross-lane-integration-sync/m227-a012-drift`",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["a012_contract_doc"] = drift_dep
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-A013-DEP-01" for failure in payload["failures"])
