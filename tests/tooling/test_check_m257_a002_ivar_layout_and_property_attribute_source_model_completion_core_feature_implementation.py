from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m257_a002_ivar_layout_and_property_attribute_source_model_completion_core_feature_implementation.py"
CONTRACT_ID = "objc3c-executable-property-ivar-source-model-completion/m257-a002-v1"
SUMMARY = ROOT / "tmp" / "reports" / "m257" / "M257-A002" / "property_ivar_source_model_completion_summary.json"


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
