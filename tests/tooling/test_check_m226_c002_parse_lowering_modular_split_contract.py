from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_c002_parse_lowering_modular_split_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_c002_parse_lowering_modular_split_contract",
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
    assert payload["mode"] == "m226-c002-parse-lowering-modular-split-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 15
    assert payload["checks_total"] == payload["checks_passed"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m226() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/reports/m226/")


def test_contract_fails_closed_when_artifacts_gate_drops_readiness_check(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    artifacts_drift = tmp_path / "objc3_frontend_artifacts.cpp"
    artifacts_drift.write_text(
        contract.ARTIFACTS["artifacts_source"]
        .read_text(encoding="utf-8")
        .replace(
            "if (!IsObjc3ParseLoweringReadinessSurfaceReady(bundle.parse_lowering_readiness_surface,\n"
            "                                                 parse_lowering_readiness_error)) {",
            "if (false) {",
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
    assert any(failure["check_id"] == "M226-C002-ART-02" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_module_drops_lowering_boundary_check(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    readiness_drift = tmp_path / "objc3_parse_lowering_readiness_surface.h"
    readiness_drift.write_text(
        contract.ARTIFACTS["readiness_surface_header"]
        .read_text(encoding="utf-8")
        .replace(
            "TryBuildObjc3LoweringIRBoundary(options.lowering, lowering_boundary, lowering_error)",
            "false",
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
    assert any(failure["check_id"] == "M226-C002-MOD-02" for failure in payload["failures"])
