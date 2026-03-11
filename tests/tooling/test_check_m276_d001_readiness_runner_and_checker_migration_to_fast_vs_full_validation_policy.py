from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m276_d001_readiness_runner_and_checker_migration_to_fast_vs_full_validation_policy.py"
SUMMARY = ROOT / "tmp" / "reports" / "m276" / "M276-D001" / "readiness_runner_fast_vs_full_migration_summary.json"
CONTRACT_ID = "objc3c-readiness-runner-fast-vs-full-migration/m276-d001-v1"


def test_m276_d001_checker_emits_summary() -> None:
    completed = subprocess.run([sys.executable, str(SCRIPT)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["next_issue"] == "M276-D003"
    assert payload["active_runner_count"] == 15
    assert payload["dynamic_summary"]["run_m262_a001_lane_a_readiness"]["mode"] == "fast"
    assert payload["dynamic_summary"]["run_m263_a001_lane_a_readiness"]["mode"] == "fast"
