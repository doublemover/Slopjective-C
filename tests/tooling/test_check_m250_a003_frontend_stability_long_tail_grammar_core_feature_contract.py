from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m250_a003_frontend_stability_long_tail_grammar_core_feature_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m250_a003_frontend_stability_long_tail_grammar_core_feature_contract",
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
    assert payload["mode"] == "m250-frontend-stability-long-tail-grammar-core-feature-contract-a003-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 35
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m250_frontend_stability_long_tail_grammar_core_feature_implementation_a003_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-frontend-stability-long-tail-grammar-core-feature-implementation/m250-a003-v1`",
            "Contract ID: `objc3c-frontend-stability-long-tail-grammar-core-feature-implementation/m250-a003-drift`",
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
    assert any(failure["check_id"] == "M250-A003-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_forces_long_tail_consistent(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_readiness = tmp_path / "objc3_parse_lowering_readiness_surface.h"
    drift_readiness.write_text(
        contract.ARTIFACTS["parse_readiness"].read_text(encoding="utf-8").replace(
            "surface.long_tail_grammar_core_feature_consistent =\n"
            "      parser_snapshot.long_tail_grammar_covered_construct_count <=\n"
            "          parser_snapshot.long_tail_grammar_construct_count &&\n"
            "      parser_snapshot.long_tail_grammar_fingerprint != 0 &&\n"
            "      !parser_snapshot.long_tail_grammar_handoff_key.empty() &&\n"
            "      parser_snapshot.long_tail_grammar_handoff_deterministic;",
            "surface.long_tail_grammar_core_feature_consistent = true;",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["parse_readiness"] = drift_readiness
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] in {"M250-A003-REA-02", "M250-A003-FORB-01"}
        for failure in payload["failures"]
    )
