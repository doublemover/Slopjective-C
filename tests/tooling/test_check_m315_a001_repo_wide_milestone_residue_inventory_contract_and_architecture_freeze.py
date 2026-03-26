from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
INVENTORY_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_a001_repo_wide_milestone_residue_inventory_contract_and_architecture_freeze_inventory.json"


def test_inventory_freezes_repo_wide_match_count() -> None:
    payload = json.loads(INVENTORY_JSON.read_text(encoding="utf-8"))
    assert payload["repo_wide_totals"]["match_count"] == 277347


def test_inventory_hands_native_slice_to_a002() -> None:
    payload = json.loads(INVENTORY_JSON.read_text(encoding="utf-8"))
    assert payload["downstream_ownership"]["repo_wide_scope_refinement"] == "M315-A002"
