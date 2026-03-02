from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m250_a002_frontend_stability_long_tail_grammar_modular_split_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m250_a002_frontend_stability_long_tail_grammar_modular_split_contract",
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
    assert payload["mode"] == "m250-frontend-stability-long-tail-grammar-modular-split-contract-a002-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 20
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m250_frontend_stability_long_tail_grammar_modular_split_a002_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-frontend-stability-long-tail-grammar-modular-split/m250-a002-v1`",
            "Contract ID: `objc3c-frontend-stability-long-tail-grammar-modular-split/m250-a002-drift`",
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
    assert any(failure["check_id"] == "M250-A002-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_parser_drops_parse_support_include(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_parser = tmp_path / "objc3_parser.cpp"
    drift_parser.write_text(
        contract.ARTIFACTS["parser_source"].read_text(encoding="utf-8").replace(
            "#include \"parse/objc3_parse_support.h\"",
            "#include \"parse/objc3_parse_support_removed.h\"",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["parser_source"] = drift_parser
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M250-A002-PRS-01" for failure in payload["failures"])
