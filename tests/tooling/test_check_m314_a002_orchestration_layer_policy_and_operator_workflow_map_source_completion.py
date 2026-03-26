from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MAP_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_a002_orchestration_layer_policy_and_operator_workflow_map_source_completion_map.json"


def test_map_freezes_primary_public_layer() -> None:
    payload = json.loads(MAP_JSON.read_text(encoding="utf-8"))
    assert payload["primary_public_layer"] == "package.json scripts"


def test_map_tracks_documented_transition_leaks() -> None:
    payload = json.loads(MAP_JSON.read_text(encoding="utf-8"))
    assert [entry["source"] for entry in payload["documented_transition_leaks"]] == [
        "README.md",
        "README.md",
        "README.md",
    ]
