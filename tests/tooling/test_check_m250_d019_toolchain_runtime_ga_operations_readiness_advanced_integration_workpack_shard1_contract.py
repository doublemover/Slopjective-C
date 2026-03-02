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
    / "check_m250_d019_toolchain_runtime_ga_operations_readiness_advanced_integration_workpack_shard1_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m250_d019_toolchain_runtime_ga_operations_readiness_advanced_integration_workpack_shard1_contract",
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
        == "m250-toolchain-runtime-ga-operations-readiness-advanced-integration-workpack-shard1-contract-d019-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 21
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = (
        tmp_path
        / "m250_toolchain_runtime_ga_operations_readiness_advanced_integration_workpack_shard1_d019_expectations.md"
    )
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-advanced-integration-workpack-shard1/m250-d019-v1`",
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-advanced-integration-workpack-shard1/m250-d019-drift`",
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
    assert any(failure["check_id"] == "M250-D019-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_surface_forces_advanced_integration_consistency(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_surface = tmp_path / "objc3_parse_lowering_readiness_surface.h"
    drift_surface.write_text(
        contract.ARTIFACTS["parse_surface_header"].read_text(encoding="utf-8")
        + "\ntoolchain_runtime_ga_operations_advanced_integration_consistent = true;\n",
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["parse_surface_header"] = drift_surface
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M250-D019-FORB-01" for failure in payload["failures"])


def test_contract_fails_closed_when_advanced_integration_failure_reason_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_surface = tmp_path / "objc3_parse_lowering_readiness_surface.h"
    drift_surface.write_text(
        contract.ARTIFACTS["parse_surface_header"].read_text(encoding="utf-8").replace(
            "toolchain/runtime GA operations advanced integration workpack is not ready",
            "toolchain/runtime GA operations advanced integration drift marker",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["parse_surface_header"] = drift_surface
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M250-D019-SUR-08" for failure in payload["failures"])
