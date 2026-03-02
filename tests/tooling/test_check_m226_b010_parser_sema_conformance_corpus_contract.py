from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_b010_parser_sema_conformance_corpus_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_b010_parser_sema_conformance_corpus_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m226_b010_parser_sema_conformance_corpus_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m226-parser-sema-conformance-corpus-contract-b010-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/")


def test_contract_fails_closed_when_handoff_corpus_builder_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_file = tmp_path / "objc3_parser_sema_handoff_scaffold.h"
    drift_file.write_text(
        contract.ARTIFACTS["sema_handoff"].read_text(encoding="utf-8").replace(
            "BuildObjc3ParserSemaConformanceCorpus(",
            "BuildObjc3ParserSemaConformanceCorpusDrift(",
            1,
        ),
        encoding="utf-8",
    )
    artifacts = dict(contract.ARTIFACTS)
    artifacts["sema_handoff"] = drift_file
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-B010-SEM-01" for failure in payload["failures"])


def test_contract_fails_closed_when_pass_manager_corpus_gate_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_file = tmp_path / "objc3_sema_pass_manager.cpp"
    drift_file.write_text(
        contract.ARTIFACTS["sema_pass_manager"].read_text(encoding="utf-8").replace(
            "result.deterministic_parser_sema_conformance_corpus =",
            "result.deterministic_parser_sema_conformance_corpus_drift =",
            1,
        ),
        encoding="utf-8",
    )
    artifacts = dict(contract.ARTIFACTS)
    artifacts["sema_pass_manager"] = drift_file
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-B010-PM-02" for failure in payload["failures"])
