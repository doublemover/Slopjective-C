from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m228_b001_ownership_aware_lowering_behavior_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m228_b001_ownership_aware_lowering_behavior_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m228_b001_ownership_aware_lowering_behavior_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m228-b001-ownership-aware-lowering-behavior-freeze-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/")


def test_contract_fails_closed_when_lane_contract_marker_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_source = tmp_path / "objc3_lowering_contract.cpp"
    drift_source.write_text(
        contract.ARTIFACTS["lowering_contract_source"].read_text(encoding="utf-8").replace(
            '";lane_contract=" + kObjc3ArcDiagnosticsFixitLoweringLaneContract;',
            '";lane_contract=" + kObjc3ArcDiagnosticsFixitLoweringLaneContract_DRIFT;',
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["lowering_contract_source"] = drift_source
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-B001-CPP-08" for failure in payload["failures"])


def test_contract_fails_closed_when_artifact_lane_contract_projection_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_artifacts = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_artifacts.write_text(
        contract.ARTIFACTS["artifacts_source"].read_text(encoding="utf-8").replace(
            '<< "\\",\\"lane_contract\\":\\"" << kObjc3AutoreleasePoolScopeLoweringLaneContract',
            '<< "\\",\\"lane_contract_drift\\":\\"" << kObjc3AutoreleasePoolScopeLoweringLaneContract',
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["artifacts_source"] = drift_artifacts
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-B001-ART-11" for failure in payload["failures"])
