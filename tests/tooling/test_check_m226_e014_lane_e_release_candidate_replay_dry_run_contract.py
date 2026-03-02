from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_e014_lane_e_release_candidate_replay_dry_run_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_e014_lane_e_release_candidate_replay_dry_run_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_m226_e014_lane_e_release_candidate_replay_dry_run_contract.py")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m226-lane-e-release-candidate-replay-dry-run-contract-e014-v1"
    assert payload["ok"] is True


def test_contract_fails_closed_when_doc_contract_id_drifts(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    drift_doc = tmp_path / "m226_lane_e_release_candidate_replay_dry_run_e014_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-lane-e-release-candidate-replay-dry-run/m226-e014-v1`",
            "Contract ID: `objc3c-lane-e-release-candidate-replay-dry-run/m226-e014-drift`",
            1,
        ),
        encoding="utf-8",
    )
    artifacts = dict(contract.ARTIFACTS)
    artifacts["contract_doc"] = drift_doc
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert any(failure["check_id"] == "M226-E014-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_surface_forced_ready_literal_present(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_surface = tmp_path / "objc3_final_readiness_gate_core_feature_implementation_surface.h"
    drift_surface.write_text(
        contract.ARTIFACTS["core_surface_header"].read_text(encoding="utf-8").replace(
            "surface.release_candidate_replay_dry_run_ready =",
            "surface.release_candidate_replay_dry_run_ready = true;\n  surface.release_candidate_replay_dry_run_ready =",
            1,
        ),
        encoding="utf-8",
    )
    artifacts = dict(contract.ARTIFACTS)
    artifacts["core_surface_header"] = drift_surface
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert any(failure["check_id"] == "M226-E014-FORB-01" for failure in payload["failures"])



