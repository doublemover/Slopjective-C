from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_e002_validation_consolidation_closeout_matrix_cross_lane_integration_sync_contract.json"


def test_closeout_contract_tracks_expected_matrix_rows() -> None:
    payload = json.loads(CONTRACT.read_text(encoding="utf-8"))
    assert len(payload["matrix_rows"]) == 14


def test_closeout_contract_freezes_precloseout_issue_state() -> None:
    payload = json.loads(CONTRACT.read_text(encoding="utf-8"))
    assert payload["pre_closeout_milestone_state"]["expected_open_issue_numbers"] == [7779]
