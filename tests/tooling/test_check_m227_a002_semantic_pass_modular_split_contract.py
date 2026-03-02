from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m227_a002_semantic_pass_modular_split_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m227_a002_semantic_pass_modular_split_contract",
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
    assert payload["mode"] == "m227-sema-pass-modular-split-scaffold-contract-a002-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 10
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m227_semantic_pass_modular_split_expectations.md"
    drift_doc.write_text(
        (
            ROOT / "docs" / "contracts" / "m227_semantic_pass_modular_split_expectations.md"
        ).read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-sema-pass-modular-split-scaffold/m227-a002-v1`",
            "Contract ID: `objc3c-sema-pass-modular-split-scaffold/m227-a002-drift`",
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
    assert any(
        failure["check_id"] == "M227-A002-DOC-01"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_pass_manager_reabsorbs_split_logic(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_source = tmp_path / "objc3_sema_pass_manager.cpp"
    drift_source.write_text(
        (
            ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
        ).read_text(encoding="utf-8")
        + "\n++result.sema_pass_flow_summary.executed_pass_count;\n",
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["sema_pass_manager_source"] = drift_source
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M227-A002-SRC-04"
        for failure in payload["failures"]
    )
