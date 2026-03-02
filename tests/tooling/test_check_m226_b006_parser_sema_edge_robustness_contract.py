from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_b006_parser_sema_edge_robustness_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_b006_parser_sema_edge_robustness_contract",
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
    assert payload["mode"] == "m226-parser-sema-edge-robustness-contract-b006-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_budget_guard_drops(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_handoff = tmp_path / "objc3_parser_sema_handoff_scaffold.h"
    drift_handoff.write_text(
        (
            ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_parser_sema_handoff_scaffold.h"
        ).read_text(encoding="utf-8").replace(
            "parser_token_top_level_budget_consistent &&",
            "true &&",
            1,
        ),
        encoding="utf-8",
    )
    artifacts = dict(contract.ARTIFACTS)
    artifacts["sema_handoff"] = drift_handoff
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M226-B006-SEM-11"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_doc_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m226_parser_sema_edge_robustness_b006_expectations.md"
    drift_doc.write_text(
        (
            ROOT / "docs" / "contracts" / "m226_parser_sema_edge_robustness_b006_expectations.md"
        ).read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-parser-sema-edge-robustness-contract/m226-b006-v1`",
            "Contract ID: `objc3c-parser-sema-edge-robustness-contract/m226-b006-drift`",
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
        failure["check_id"] == "M226-B006-DOC-01"
        for failure in payload["failures"]
    )
