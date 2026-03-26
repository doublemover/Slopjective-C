from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
RESULT_JSON = ROOT / "spec" / "planning" / "compiler" / "m315" / "m315_b003_milestone_marker_removal_from_native_source_core_feature_implementation_result.json"


def test_result_freezes_zero_leading_comment_markers() -> None:
    payload = json.loads(RESULT_JSON.read_text(encoding="utf-8"))
    assert payload["post_state"]["leading_comment_markers_remaining"] == 0


def test_result_hands_remaining_edge_sweep_to_b005() -> None:
    payload = json.loads(RESULT_JSON.read_text(encoding="utf-8"))
    assert payload["downstream_ownership"]["remaining_edge_sweep"] == "M315-B005"
