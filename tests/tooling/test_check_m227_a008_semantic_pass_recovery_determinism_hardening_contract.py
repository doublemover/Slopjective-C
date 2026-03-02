from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m227_a008_semantic_pass_recovery_determinism_hardening_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m227_a008_semantic_pass_recovery_determinism_hardening_contract",
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
    assert payload["mode"] == "m227-sema-pass-recovery-determinism-hardening-a008-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 24
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_recovery_key_determinism_gate_is_removed(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_contract = tmp_path / "objc3_sema_pass_manager_contract.h"
    drift_contract.write_text(
        (
            ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
        ).read_text(encoding="utf-8").replace(
            "surface.pass_flow_recovery_replay_key_deterministic &&",
            "surface.pass_flow_recovery_replay_key_deterministic_removed &&",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["sema_contract"] = drift_contract
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-A008-CNT-10" for failure in payload["failures"])


def test_contract_fails_closed_when_artifact_projection_drops_recovery_field(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_artifacts = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_artifacts.write_text(
        (
            ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
        ).read_text(encoding="utf-8").replace(
            "pass_flow_parser_recovery_replay_case_present",
            "pf_prrcp_removed",
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["artifact_projection"] = drift_artifacts
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-A008-ART-06" for failure in payload["failures"])
