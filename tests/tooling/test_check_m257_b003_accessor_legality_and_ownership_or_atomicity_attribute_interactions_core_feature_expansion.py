from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m257_b003_accessor_legality_and_ownership_or_atomicity_attribute_interactions_core_feature_expansion.py"
CONTRACT_ID = "objc3c-property-accessor-attribute-interactions/m257-b003-v1"
SUMMARY = ROOT / "tmp" / "reports" / "m257" / "M257-B003" / "accessor_legality_attribute_interactions_summary.json"


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
