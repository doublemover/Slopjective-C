from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_a026_parser_advanced_performance_workpack_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_a026_parser_advanced_performance_workpack_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m226_a026_parser_advanced_performance_workpack_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m226-parser-advanced-performance-workpack-contract-a026-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_run_script_non_gating_anchor_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_script = tmp_path / "run_m226_a026_parser_performance_workpack.ps1"
    drift_script.write_text(
        (
            ROOT / "scripts" / "run_m226_a026_parser_performance_workpack.ps1"
        ).read_text(encoding="utf-8").replace(
            "non_gating = $true",
            "non_gating_drift = $true",
            1,
        ),
        encoding="utf-8",
    )
    artifacts = dict(contract.ARTIFACTS)
    artifacts["run_script"] = drift_script
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-A026-RUN-09" for failure in payload["failures"])


def test_contract_fails_closed_when_doc_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m226_parser_advanced_performance_workpack_a026_expectations.md"
    drift_doc.write_text(
        (
            ROOT / "docs" / "contracts" / "m226_parser_advanced_performance_workpack_a026_expectations.md"
        ).read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-parser-advanced-performance-workpack-contract/m226-a026-v1`",
            "Contract ID: `objc3c-parser-advanced-performance-workpack-contract/m226-a026-drift`",
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
    assert any(failure["check_id"] == "M226-A026-DOC-01" for failure in payload["failures"])
