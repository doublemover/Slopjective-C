from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m264_b003_canonical_interface_and_feature_macro_truthfulness_edge_case_and_compatibility_completion.py"
TEST_SUMMARY = ROOT / "tmp" / "reports" / "tests" / "m264_b003_canonical_interface_and_feature_macro_truthfulness_summary.json"


def test_m264_b003_checker_emits_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT), "--summary-out", str(TEST_SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(TEST_SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c-canonical-interface-and-feature-macro-truthfulness/m264-b003-v1"
    assert payload["checks_total"] == payload["checks_passed"]
    assert payload["failures"] == []
    assert len(payload["cases"]) == 3
