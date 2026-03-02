from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_a012_cross_lane_integration_sync_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_a012_cross_lane_integration_sync_contract",
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
    assert payload["mode"] == "m226-cross-lane-integration-sync-contract-a012-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_a012_doc_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m226_cross_lane_integration_sync_a012_expectations.md"
    drift_doc.write_text(
        (
            ROOT / "docs" / "contracts" / "m226_cross_lane_integration_sync_a012_expectations.md"
        ).read_text(encoding="utf-8").replace(
            "objc3c-frontend-build-invocation-manifest-guard/m226-d003-v1",
            "objc3c-frontend-build-invocation-manifest-guard/m226-d003-drift",
            1,
        ),
        encoding="utf-8",
    )
    monkeypatch.setattr(contract, "CONTRACT_DOC", drift_doc)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M226-A012-LINK-D003"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_lane_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_lane = tmp_path / "m226_parser_sema_core_handoff_b003_expectations.md"
    drift_lane.write_text(
        (
            ROOT / "docs" / "contracts" / "m226_parser_sema_core_handoff_b003_expectations.md"
        ).read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-parser-sema-core-handoff-contract/m226-b003-v1`",
            "Contract ID: `objc3c-parser-sema-core-handoff-contract/m226-b003-drift`",
            1,
        ),
        encoding="utf-8",
    )
    lane_contracts = list(contract.LANE_CONTRACTS)
    for idx, item in enumerate(lane_contracts):
        packet, contract_id, path = item
        if packet == "B003":
            lane_contracts[idx] = (packet, contract_id, drift_lane)
            break
    monkeypatch.setattr(contract, "LANE_CONTRACTS", tuple(lane_contracts))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M226-A012-LANE-B003"
        for failure in payload["failures"]
    )
