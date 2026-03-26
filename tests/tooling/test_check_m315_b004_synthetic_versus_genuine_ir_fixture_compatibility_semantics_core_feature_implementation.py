from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REGISTRY_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_b004_synthetic_versus_genuine_ir_fixture_compatibility_semantics_core_feature_implementation_registry.json"


def test_registry_freezes_tracked_ll_totals() -> None:
    payload = json.loads(REGISTRY_JSON.read_text(encoding="utf-8"))
    assert payload["ll_fixture_totals"]["tracked_ll_files"] == 78


def test_registry_hands_regeneration_to_c003() -> None:
    payload = json.loads(REGISTRY_JSON.read_text(encoding="utf-8"))
    assert payload["downstream_ownership"]["replay_regeneration"] == "M315-C003"
