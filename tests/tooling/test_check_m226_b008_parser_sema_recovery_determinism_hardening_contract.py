from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_b008_parser_sema_recovery_determinism_hardening_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_b008_parser_sema_recovery_determinism_hardening_contract",
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
    assert payload["mode"] == "m226-parser-sema-recovery-determinism-hardening-contract-b008-v1"
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
            "if (!recovery_and_block_determinism_hardening_consistent) {",
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
        failure["check_id"] == "M226-B008-SEM-07"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_readiness_recovery_bound_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_contract = tmp_path / "objc3_sema_pass_manager_contract.h"
    drift_contract.write_text(
        (
            ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
        ).read_text(encoding="utf-8").replace(
            ".fail_closed_diagnostic_sites <=\n"
            "             surface.error_diagnostics_recovery_summary\n"
            "                 .diagnostic_emit_sites &&",
            ".fail_closed_diagnostic_sites <=\n"
            "             surface.error_diagnostics_recovery_summary\n"
            "                 .error_diagnostics_recovery_sites &&",
            1,
        ),
        encoding="utf-8",
    )
    artifacts = dict(contract.ARTIFACTS)
    artifacts["sema_pass_manager_contract"] = drift_contract
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M226-B008-CON-01"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_doc_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m226_parser_sema_recovery_determinism_hardening_b008_expectations.md"
    drift_doc.write_text(
        (
            ROOT / "docs" / "contracts" / "m226_parser_sema_recovery_determinism_hardening_b008_expectations.md"
        ).read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-parser-sema-recovery-determinism-hardening-contract/m226-b008-v1`",
            "Contract ID: `objc3c-parser-sema-recovery-determinism-hardening-contract/m226-b008-drift`",
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
        failure["check_id"] == "M226-B008-DOC-01"
        for failure in payload["failures"]
    )
