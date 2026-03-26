from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys


REPO_ROOT = Path(__file__).resolve().parents[2]
CHECKER = (
    REPO_ROOT
    / "scripts"
    / "check_m315_c003_genuine_replay_artifact_regeneration_and_provenance_capture_core_feature_implementation.py"
)
SUMMARY_PATH = (
    REPO_ROOT
    / "tmp"
    / "reports"
    / "m315"
    / "M315-C003"
    / "genuine_replay_provenance_capture_summary.json"
)


def test_m315_c003_checker_passes_and_writes_expected_summary() -> None:
    result = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert result.returncode == 0, result.stderr
    summary = json.loads(SUMMARY_PATH.read_text(encoding="utf-8"))
    assert summary["legacy_bridge_total"] == 76
    assert summary["legacy_bridge_with_frontend_header"] == 30
    assert summary["legacy_bridge_without_frontend_header"] == 46
    assert summary["live_generated_replay"]["header_present"] is True
    assert summary["live_generated_replay"]["ir_contains_main"] is True
    assert summary["ok"] is True
