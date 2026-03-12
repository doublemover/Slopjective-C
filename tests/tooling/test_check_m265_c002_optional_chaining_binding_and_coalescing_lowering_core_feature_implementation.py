from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SUMMARY = ROOT / "tmp" / "reports" / "m265" / "M265-C002" / "optional_chaining_binding_and_coalescing_lowering_summary.json"


def test_summary_exists_and_passes() -> None:
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["dynamic_probes_executed"] is True
