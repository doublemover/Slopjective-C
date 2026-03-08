from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = (
    ROOT
    / "scripts"
    / "check_m253_d003_archive_and_static_link_metadata_discovery_behavior_edge_case_and_compatibility_completion.py"
)
SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m253"
    / "M253-D003"
    / "archive_and_static_link_metadata_discovery_behavior_summary.json"
)


def test_checker_passes() -> None:
    result = subprocess.run(
        [sys.executable, str(SCRIPT)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )
    assert result.returncode == 0, result.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["mode"] == "m253-d003"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    cases = {case["case_id"]: case for case in payload["dynamic_cases"]}
    collision = cases["M253-D003-CASE-COLLISION-PROVENANCE"]
    assert collision["anchor_symbols"][0] != collision["anchor_symbols"][1]
    assert collision["discovery_symbols"][0] != collision["discovery_symbols"][1]
    fanin = cases["M253-D003-CASE-MULTI-ARCHIVE-FANIN"]
    assert fanin["plain_link_exit_code"] == 0
    assert fanin["merged_link_exit_code"] == 0
    assert fanin["merged_retained_metadata_raw_size"] > fanin["single_retained_metadata_raw_size"]
    negative = cases["M253-D003-CASE-NEGATIVE-COLLIDING-DISCOVERY-INPUTS"]
    assert negative["merge_exit_code"] != 0
    assert negative["merged_response_exists"] is False
    assert negative["merged_discovery_exists"] is False
