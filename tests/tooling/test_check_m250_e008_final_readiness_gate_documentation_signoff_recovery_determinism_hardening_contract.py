from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m250_e008_final_readiness_gate_documentation_signoff_recovery_determinism_hardening_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m250_e008_final_readiness_gate_documentation_signoff_recovery_determinism_hardening_contract",
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
    assert (
        payload["mode"]
        == "m250-final-readiness-gate-documentation-signoff-recovery-determinism-hardening-contract-e008-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 40
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = (
        tmp_path
        / "m250_final_readiness_gate_documentation_signoff_recovery_determinism_hardening_e008_expectations.md"
    )
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-recovery-determinism-hardening/m250-e008-v1`",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-recovery-determinism-hardening/m250-e008-drift`",
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
    assert any(failure["check_id"] == "M250-E008-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_surface_forces_recovery_ready(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_surface = tmp_path / "objc3_final_readiness_gate_core_feature_implementation_surface.h"
    drift_surface.write_text(
        contract.ARTIFACTS["core_surface_header"].read_text(encoding="utf-8")
        + "\nsurface.recovery_determinism_ready = true;\n",
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["core_surface_header"] = drift_surface
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] in {"M250-E008-SUR-06", "M250-E008-FORB-01"}
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_recovery_failure_reason_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_surface = tmp_path / "objc3_final_readiness_gate_core_feature_implementation_surface.h"
    drift_surface.write_text(
        contract.ARTIFACTS["core_surface_header"].read_text(encoding="utf-8").replace(
            "final readiness gate recovery and determinism hardening is not ready",
            "final readiness gate recovery drift marker",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["core_surface_header"] = drift_surface
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M250-E008-SUR-15" for failure in payload["failures"])
