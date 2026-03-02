from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_b002_parser_sema_modular_handoff_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_b002_parser_sema_modular_handoff_contract",
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
    assert payload["mode"] == "m226-b002-parser-sema-modular-handoff-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 20
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_fails_closed_when_pass_manager_drops_handoff_guard(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    sema_source_drift = tmp_path / "objc3_sema_pass_manager.cpp"
    sema_source_drift.write_text(
        (ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp")
        .read_text(encoding="utf-8")
        .replace(
            "if (!handoff.deterministic) {",
            "if (false) {",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["sema_pass_manager_source"] = sema_source_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)
    monkeypatch.setattr(
        contract,
        "ARTIFACT_ORDER",
        tuple(artifact_overrides.keys()),
    )
    monkeypatch.setattr(
        contract,
        "ARTIFACT_RANK",
        {name: index for index, name in enumerate(contract.ARTIFACT_ORDER)},
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M226-B002-SEM-08" for failure in payload["failures"]
    )


def test_contract_fails_closed_when_sema_input_loses_parser_snapshot_pointer(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    sema_contract_drift = tmp_path / "objc3_sema_pass_manager_contract.h"
    sema_contract_drift.write_text(
        (ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h")
        .read_text(encoding="utf-8")
        .replace(
            "  const Objc3ParserContractSnapshot *parser_contract_snapshot = nullptr;\n",
            "",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["sema_pass_manager_contract_header"] = sema_contract_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)
    monkeypatch.setattr(
        contract,
        "ARTIFACT_ORDER",
        tuple(artifact_overrides.keys()),
    )
    monkeypatch.setattr(
        contract,
        "ARTIFACT_RANK",
        {name: index for index, name in enumerate(contract.ARTIFACT_ORDER)},
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M226-B002-SEM-01" for failure in payload["failures"]
    )
