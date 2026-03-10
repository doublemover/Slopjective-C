from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = (
    ROOT
    / "scripts"
    / "check_m256_d004_canonical_runnable_class_and_object_sample_support_core_feature_expansion.py"
)
SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m256"
    / "M256-D004"
    / "canonical_runnable_object_sample_support_summary.json"
)
CONTRACT_ID = "objc3c-runtime-canonical-runnable-object-sample-support/m256-d004-v1"


def test_m256_d004_checker_static_contract() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["dynamic_probes_executed"] is False
