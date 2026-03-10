from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m257_b002_property_synthesis_and_default_ivar_binding_full_semantics_core_feature_implementation.py"
CONTRACT_ID = "objc3c-property-default-ivar-binding-semantics/m257-b002-v1"
SUMMARY = ROOT / "tmp" / "reports" / "m257" / "M257-B002" / "property_synthesis_default_ivar_binding_full_semantics_summary.json"


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
