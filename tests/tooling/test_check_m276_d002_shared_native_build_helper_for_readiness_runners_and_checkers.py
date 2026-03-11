from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m276_d002_shared_native_build_helper_for_readiness_runners_and_checkers.py"
SUMMARY = ROOT / "tmp" / "reports" / "m276" / "M276-D002" / "shared_native_build_helper_summary.json"
CONTRACT_ID = "objc3c-shared-native-build-helper/m276-d002-v1"


def test_m276_d002_checker_emits_summary() -> None:
    completed = subprocess.run([sys.executable, str(SCRIPT)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["next_issue"] == "M276-D001"
    assert payload["dynamic_summary"]["fast"]["execution_mode"] == "binaries-only"
    assert payload["dynamic_summary"]["contracts"]["execution_mode"] == "packets-binary"
    assert payload["dynamic_summary"]["full"]["execution_mode"] == "full"
    assert payload["dynamic_summary"]["full"]["saw_cmake_configure_force_reconfigure"] is True
