from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m227_c025_typed_sema_to_lowering_advanced_integration_shard2_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m227_c025_typed_sema_to_lowering_advanced_integration_shard2_contract",
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
    assert payload["mode"] == "m227-typed-sema-to-lowering-advanced-integration-shard2-c025-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_when_typed_surface_advanced_integration_shard2_assignment_drifts(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    drift_surface = tmp_path / "objc3_typed_sema_to_lowering_contract_surface.h"
    drift_surface.write_text(
        (
            ROOT
            / "native"
            / "objc3c"
            / "src"
            / "pipeline"
            / "objc3_typed_sema_to_lowering_contract_surface.h"
        ).read_text(encoding="utf-8").replace(
            "surface.typed_advanced_integration_shard2_key =",
            "surface.typed_advanced_integration_shard2_key_removed =",
            1,
        ),
        encoding="utf-8",
    )
    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["typed_surface"] = drift_surface
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-C025-SUR-04" for failure in payload["failures"])


def test_contract_fails_when_readiness_advanced_integration_shard2_alignment_drifts(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    drift_readiness = tmp_path / "objc3_parse_lowering_readiness_surface.h"
    drift_readiness.write_text(
        (
            ROOT
            / "native"
            / "objc3c"
            / "src"
            / "pipeline"
            / "objc3_parse_lowering_readiness_surface.h"
        ).read_text(encoding="utf-8").replace(
            "const bool typed_advanced_integration_shard2_alignment =",
            "const bool typed_advanced_integration_shard2_alignment_removed =",
            1,
        ),
        encoding="utf-8",
    )
    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["parse_readiness"] = drift_readiness
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-C025-REA-04" for failure in payload["failures"])





