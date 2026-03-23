from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m271_a002_frontend_cleanup_resource_and_capture_surface_completion_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m271" / "M271-A002" / "pytest_checker_summary.json"


def test_m271_a002_checker_static() -> None:
    result = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes", "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert result.returncode == 0, result.stderr or result.stdout
    assert SUMMARY.exists()
