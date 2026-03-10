from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m257_c003_synthesized_accessor_and_property_metadata_lowering_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m257" / "M257-C003" / "synthesized_accessor_property_lowering_summary.json"
CONTRACT_ID = "objc3c-executable-synthesized-accessor-property-lowering/m257-c003-v1"


def test_checker_passes() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["dynamic_probes_executed"] is False
