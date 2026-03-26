from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
INVENTORY_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_a003_artifact_authenticity_and_provenance_classification_inventory_source_completion_inventory.json"


def test_inventory_freezes_artifact_candidate_total() -> None:
    payload = json.loads(INVENTORY_JSON.read_text(encoding="utf-8"))
    assert payload["totals"]["tracked_artifact_candidates"] == 2514


def test_inventory_hands_replay_capture_to_c003() -> None:
    payload = json.loads(INVENTORY_JSON.read_text(encoding="utf-8"))
    assert payload["downstream_ownership"]["replay_regeneration_and_provenance_capture"] == "M315-C003"


def test_inventory_records_synthetic_fixture_authenticity_envelopes() -> None:
    payload = json.loads(INVENTORY_JSON.read_text(encoding="utf-8"))
    assert payload["provenance_signal_counts"]["artifact_authenticity_envelope"] == 3
