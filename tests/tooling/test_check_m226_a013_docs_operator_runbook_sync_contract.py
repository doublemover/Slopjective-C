from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_a013_docs_operator_runbook_sync_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_a013_docs_operator_runbook_sync_contract",
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
    assert payload["mode"] == "m226-docs-operator-runbook-sync-contract-a013-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_runbook_anchor_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_runbook = tmp_path / "m226_wave_execution_runbook.md"
    drift_runbook.write_text(
        (
            ROOT / "docs" / "runbooks" / "m226_wave_execution_runbook.md"
        ).read_text(encoding="utf-8").replace(
            "objc3c-frontend-build-invocation-manifest-guard/m226-d003-v1",
            "objc3c-frontend-build-invocation-manifest-guard/m226-d003-drift",
            1,
        ),
        encoding="utf-8",
    )
    artifacts = dict(contract.ARTIFACTS)
    artifacts["runbook"] = drift_runbook
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M226-A013-RUN-04"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_contract_doc_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m226_docs_operator_runbook_sync_a013_expectations.md"
    drift_doc.write_text(
        (
            ROOT / "docs" / "contracts" / "m226_docs_operator_runbook_sync_a013_expectations.md"
        ).read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-docs-operator-runbook-sync-contract/m226-a013-v1`",
            "Contract ID: `objc3c-docs-operator-runbook-sync-contract/m226-a013-drift`",
            1,
        ),
        encoding="utf-8",
    )
    artifacts = dict(contract.ARTIFACTS)
    artifacts["contract_doc"] = drift_doc
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M226-A013-DOC-01"
        for failure in payload["failures"]
    )
