from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_a011_parser_performance_quality_guardrails_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_a011_parser_performance_quality_guardrails_contract",
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
    assert payload["mode"] == "m226-parser-performance-quality-guardrails-contract-a011-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_guardrail_budget_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    budgets = dict(contract.MAX_FUNCTION_LINES)
    budgets["ParseCStyleCompatType"] = 1
    monkeypatch.setattr(contract, "MAX_FUNCTION_LINES", budgets)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        "M226-A011-LIMIT-ParseCStyleCompatType" == failure["check_id"]
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_doc_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m226_parser_performance_quality_guardrails_expectations.md"
    drift_doc.write_text(
        (
            ROOT
            / "docs"
            / "contracts"
            / "m226_parser_performance_quality_guardrails_expectations.md"
        ).read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-parser-performance-quality-guardrails-contract/m226-a011-v1`",
            "Contract ID: `objc3c-parser-performance-quality-guardrails-contract/m226-a011-drift`",
            1,
        ),
        encoding="utf-8",
    )
    monkeypatch.setattr(contract, "CONTRACT_PATH", drift_doc)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        "M226-A011-DOC-01" == failure["check_id"]
        for failure in payload["failures"]
    )
