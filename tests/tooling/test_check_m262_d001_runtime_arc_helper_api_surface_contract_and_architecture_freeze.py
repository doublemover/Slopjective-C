from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m262_d001_runtime_arc_helper_api_surface_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m262" / "M262-D001" / "runtime_arc_helper_api_surface_contract_pytest_summary.json"


def test_checker_emits_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes", "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    assert SUMMARY.exists()
    text = SUMMARY.read_text(encoding="utf-8")
    assert '"contract_id": "objc3c-runtime-arc-helper-api-surface-freeze/m262-d001-v1"' in text
    assert '"issue": "M262-D001"' in text
