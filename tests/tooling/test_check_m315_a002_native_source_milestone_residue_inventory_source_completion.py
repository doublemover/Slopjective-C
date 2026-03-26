from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
INVENTORY_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_a002_native_source_milestone_residue_inventory_source_completion_inventory.json"


def test_inventory_freezes_native_source_total() -> None:
    payload = json.loads(INVENTORY_JSON.read_text(encoding="utf-8"))
    assert payload["totals"]["match_count"] == 1809
    assert payload["totals"]["file_count_with_matches"] == 11
    assert payload["subdirectory_counts"].get("sema", 0) == 0


def test_inventory_hands_removal_to_b003() -> None:
    payload = json.loads(INVENTORY_JSON.read_text(encoding="utf-8"))
    assert payload["downstream_ownership"]["native_source_marker_removal"] == "M315-B003"
