from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
INVENTORY = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_a001_command_surface_inventory_and_script_budget_policy_contract_and_architecture_freeze_inventory.json"


def test_inventory_freezes_public_command_budget() -> None:
    payload = json.loads(INVENTORY.read_text(encoding="utf-8"))
    assert payload["public_command_budget"]["maximum_total_public_entrypoints"] == 25


def test_inventory_tracks_dead_prototype_source_path() -> None:
    payload = json.loads(INVENTORY.read_text(encoding="utf-8"))
    assert payload["prototype_surface"]["source_files"] == ["compiler/objc3c/semantic.py"]
