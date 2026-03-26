from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GATE_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_e001_workflow_simplification_gate_contract_and_architecture_freeze_gate.json"


def test_gate_tracks_eleven_predecessor_summaries() -> None:
    payload = json.loads(GATE_JSON.read_text(encoding="utf-8"))
    assert len(payload["predecessor_summaries"]) == 11


def test_gate_next_issue_is_e002() -> None:
    payload = json.loads(GATE_JSON.read_text(encoding="utf-8"))
    assert payload["next_issue"] == "M314-E002"
