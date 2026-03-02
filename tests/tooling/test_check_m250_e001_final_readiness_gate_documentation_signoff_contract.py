from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m250_e001_final_readiness_gate_documentation_signoff_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m250_e001_final_readiness_gate_documentation_signoff_contract",
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
    assert payload["mode"] == "m250-final-readiness-gate-documentation-signoff-contract-freeze-e001-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 18
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m250_final_readiness_gate_documentation_signoff_contract_freeze_e001_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-freeze/m250-e001-v1`",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-freeze/m250-e001-drift`",
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
    assert any(failure["check_id"] == "M250-E001-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_dependency_packet_drops_d001(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_packet = tmp_path / "m250_e001_final_readiness_gate_documentation_signoff_contract_freeze_packet.md"
    drift_packet.write_text(
        contract.ARTIFACTS["packet_doc"].read_text(encoding="utf-8").replace(
            "Dependencies: `M250-A001`, `M250-B001`, `M250-C001`, `M250-D001`",
            "Dependencies: `M250-A001`, `M250-B001`, `M250-C001`",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["packet_doc"] = drift_packet
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M250-E001-PKT-02" for failure in payload["failures"])
