from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT / "scripts" / "check_m226_c019_parse_lowering_advanced_integration_workpack_shard1_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m226_c019_parse_lowering_advanced_integration_workpack_shard1_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m226_c019_parse_lowering_advanced_integration_workpack_shard1_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m226-c019-parse-lowering-advanced-integration-workpack-shard1-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/")


def test_contract_fails_closed_when_surface_loses_advanced_integration_assignment(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    readiness_drift = tmp_path / "objc3_parse_lowering_readiness_surface.h"
    readiness_drift.write_text(
        contract.ARTIFACTS["readiness_surface_header"].read_text(encoding="utf-8").replace(
            "surface.toolchain_runtime_ga_operations_advanced_integration_consistent =",
            "surface.toolchain_runtime_ga_operations_advanced_integration_drift =",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["readiness_surface_header"] = readiness_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-C019-RDY-04" for failure in payload["failures"])


def test_contract_fails_closed_when_manifest_loses_advanced_integration_key_projection(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    artifacts_drift = tmp_path / "objc3_frontend_artifacts.cpp"
    artifacts_drift.write_text(
        contract.ARTIFACTS["artifacts_source"].read_text(encoding="utf-8").replace(
            '\\"toolchain_runtime_ga_operations_advanced_integration_key\\":\\"',
            '\\"toolchain_runtime_ga_operations_advanced_integration_key_drift\\":\\"',
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["artifacts_source"] = artifacts_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-C019-ART-03" for failure in payload["failures"])
