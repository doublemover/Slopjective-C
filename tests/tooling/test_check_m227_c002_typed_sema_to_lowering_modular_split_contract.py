from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m227_c002_typed_sema_to_lowering_modular_split_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m227_c002_typed_sema_to_lowering_modular_split_contract",
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
    assert payload["mode"] == "m227-c002-typed-sema-to-lowering-modular-split-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 15
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m227() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/reports/m227/")


def test_contract_fails_closed_when_pipeline_drops_typed_surface_build(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    pipeline_drift = tmp_path / "objc3_frontend_pipeline.cpp"
    pipeline_drift.write_text(
        contract.ARTIFACTS["pipeline_source"]
        .read_text(encoding="utf-8")
        .replace(
            "BuildObjc3TypedSemaToLoweringContractSurface(result, options);",
            "Objc3TypedSemaToLoweringContractSurface{};",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["pipeline_source"] = pipeline_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-C002-PIP-02" for failure in payload["failures"])


def test_contract_fails_closed_when_parse_readiness_drops_typed_handoff_gate(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    readiness_drift = tmp_path / "objc3_parse_lowering_readiness_surface.h"
    readiness_drift.write_text(
        contract.ARTIFACTS["parse_readiness_header"]
        .read_text(encoding="utf-8")
        .replace(
            "typed_sema_to_lowering_contract_surface.ready_for_lowering;",
            "surface.semantic_integration_surface_built;",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["parse_readiness_header"] = readiness_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-C002-PR-04" for failure in payload["failures"])
