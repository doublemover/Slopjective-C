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
    / "check_m250_c013_lowering_runtime_stability_integration_closeout_signoff_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m250_c013_lowering_runtime_stability_integration_closeout_signoff_contract",
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
        == "m250-lowering-runtime-stability-integration-closeout-signoff-contract-c013-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 24
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = (
        tmp_path / "m250_lowering_runtime_stability_integration_closeout_signoff_c013_expectations.md"
    )
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-lowering-runtime-stability-integration-closeout-signoff/m250-c013-v1`",
            "Contract ID: `objc3c-lowering-runtime-stability-integration-closeout-signoff/m250-c013-drift`",
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
    assert any(failure["check_id"] == "M250-C013-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_surface_forces_gate_signoff_ready(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_surface = tmp_path / "objc3_lowering_runtime_stability_core_feature_implementation_surface.h"
    drift_surface.write_text(
        contract.ARTIFACTS["core_surface"].read_text(encoding="utf-8")
        + "\nconst bool gate_signoff_ready = true;\n",
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["core_surface"] = drift_surface
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] in {"M250-C013-SUR-02", "M250-C013-FORB-01"}
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_integration_closeout_expansion_failure_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_surface = tmp_path / "objc3_lowering_runtime_stability_core_feature_implementation_surface.h"
    drift_surface.write_text(
        contract.ARTIFACTS["core_surface"].read_text(encoding="utf-8").replace(
            "lowering/runtime integration closeout expansion is not ready",
            "lowering/runtime integration closeout expansion drift marker",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["core_surface"] = drift_surface
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M250-C013-SUR-14" for failure in payload["failures"])
