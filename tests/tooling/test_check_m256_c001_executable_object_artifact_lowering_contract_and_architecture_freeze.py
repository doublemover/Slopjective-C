from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m256_c001_executable_object_artifact_lowering_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m256" / "M256-C001" / "executable_object_artifact_lowering_contract_summary.json"
CONTRACT_ID = "objc3c-executable-object-artifact-lowering/m256-c001-v1"


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
