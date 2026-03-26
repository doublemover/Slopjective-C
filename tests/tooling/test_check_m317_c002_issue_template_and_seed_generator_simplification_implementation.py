from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SEED = ROOT / "tmp" / "planning" / "cleanup_acceleration_program" / "cleanup_acceleration_program_seed.json"


def test_regenerated_seed_uses_validation_posture() -> None:
    payload = json.loads(SEED.read_text(encoding="utf-8"))
    issues = [issue for milestone in payload["milestones"] for issue in milestone["issues"]]
    assert issues
    assert all("## Validation posture" in issue["body"] for issue in issues)


def test_regenerated_seed_drops_required_deliverables_boilerplate() -> None:
    payload = json.loads(SEED.read_text(encoding="utf-8"))
    issues = [issue for milestone in payload["milestones"] for issue in milestone["issues"]]
    assert all("## Required deliverables" not in issue["body"] for issue in issues)
