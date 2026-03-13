from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m267_b003_bridging_legality_and_diagnostic_completion_edge_case_and_compatibility_completion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m267" / "M267-B003" / "pytest_static_summary.json"


def test_m267_b003_static_checker_passes() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes", "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["checks_failed"] == 0
    assert payload["checks_passed"] == payload["checks_total"]
