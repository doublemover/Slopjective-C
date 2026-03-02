from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_b009_parser_sema_conformance_matrix_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_b009_parser_sema_conformance_matrix_contract",
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
    assert payload["mode"] == "m226-parser-sema-conformance-matrix-contract-b009-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_fail_closed_gate_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_source = tmp_path / "objc3_sema_pass_manager.cpp"
    drift_source.write_text(
        (
            ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
        ).read_text(encoding="utf-8").replace(
            "if (!result.deterministic_parser_sema_conformance_matrix) {",
            "if (false) {",
            1,
        ),
        encoding="utf-8",
    )
    artifacts = dict(contract.ARTIFACTS)
    artifacts["sema_pass_manager"] = drift_source
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M226-B009-PASS-03"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_matrix_row_logic_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_source = tmp_path / "objc3_parser_sema_handoff_scaffold.h"
    drift_source.write_text(
        (
            ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_parser_sema_handoff_scaffold.h"
        ).read_text(encoding="utf-8").replace(
            "matrix.parser_subset_count_consistent =",
            "matrix.parser_subset_matrix_consistent =",
            1,
        ),
        encoding="utf-8",
    )
    artifacts = dict(contract.ARTIFACTS)
    artifacts["sema_handoff_scaffold"] = drift_source
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M226-B009-SEM-03"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_doc_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m226_parser_sema_conformance_matrix_b009_expectations.md"
    drift_doc.write_text(
        (
            ROOT / "docs" / "contracts" / "m226_parser_sema_conformance_matrix_b009_expectations.md"
        ).read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-parser-sema-conformance-matrix-contract/m226-b009-v1`",
            "Contract ID: `objc3c-parser-sema-conformance-matrix-contract/m226-b009-drift`",
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
        failure["check_id"] == "M226-B009-DOC-01"
        for failure in payload["failures"]
    )
