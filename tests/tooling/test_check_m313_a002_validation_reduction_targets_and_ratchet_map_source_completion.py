from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
RATCHET_MAP = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_a002_validation_reduction_targets_and_ratchet_map_source_completion_ratchet_map.json"


def test_ratchet_map_freezes_expected_stage_order() -> None:
    payload = json.loads(RATCHET_MAP.read_text(encoding="utf-8"))
    assert [entry["owner_issue"] for entry in payload["ratchet_stages"]] == ["M313-A003", "M313-B004", "M313-C003", "M313-E001"]


def test_ratchet_map_freezes_closeout_caps() -> None:
    payload = json.loads(RATCHET_MAP.read_text(encoding="utf-8"))
    assert payload["closeout_maximums"] == {"check_scripts": 558, "readiness_runners": 179, "pytest_check_files": 555}
    assert payload["no_new_growth_without_exception"] is True
    assert payload["next_issue"] == "M313-A003"
