from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m276_e002_ci_adoption_closeout_evidence_and_fallback_hardening_for_incremental_native_builds.py"
SUMMARY = ROOT / "tmp" / "reports" / "m276" / "M276-E002" / "incremental_build_closeout_summary.json"
CONTRACT_ID = "objc3c-incremental-native-build-closeout/m276-e002-v1"


def test_m276_e002_checker_emits_summary() -> None:
    if not SUMMARY.exists():
        completed = subprocess.run([sys.executable, str(SCRIPT)], cwd=ROOT, capture_output=True, text=True, check=False)
        assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    dynamic = payload["dynamic_summary"]
    assert dynamic["cold_log"].endswith("cold_fast_build.log")
    assert dynamic["warm_log"].endswith("warm_fast_build.log")
    assert dynamic["contracts_log"].endswith("contracts_only_build.log")
    assert dynamic["full_log"].endswith("full_build.log")
