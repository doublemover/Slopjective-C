from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MATRIX_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_e002_command_surface_closeout_matrix_cross_lane_integration_sync_matrix.json"


def test_matrix_tracks_twelve_predecessor_summaries() -> None:
    payload = json.loads(MATRIX_JSON.read_text(encoding="utf-8"))
    assert len(payload["predecessor_summaries"]) == 12


def test_matrix_freezes_preclose_open_issue_set() -> None:
    payload = json.loads(MATRIX_JSON.read_text(encoding="utf-8"))
    assert payload["live_rules"]["milestone_open_issue_numbers_preclose"] == [7792]
