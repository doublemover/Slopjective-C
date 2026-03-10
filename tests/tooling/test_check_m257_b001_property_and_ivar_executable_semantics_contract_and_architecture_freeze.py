from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m257_b001_property_and_ivar_executable_semantics_contract_and_architecture_freeze.py"
CONTRACT_ID = "objc3c-executable-property-ivar-semantics/m257-b001-v1"
SUMMARY = ROOT / "tmp" / "reports" / "m257" / "M257-B001" / "property_ivar_executable_semantics_contract_summary.json"


def test_checker_passes() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    assert CONTRACT_ID in SUMMARY.read_text(encoding="utf-8")
