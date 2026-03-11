from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m276_d003_historical_runner_compatibility_strategy_during_build_surface_migration.py"
SUMMARY = ROOT / "tmp" / "reports" / "m276" / "M276-D003" / "historical_runner_build_surface_compatibility_summary.json"
CONTRACT_ID = "objc3c-historical-runner-build-surface-compatibility/m276-d003-v1"


def test_m276_d003_checker_emits_summary() -> None:
    completed = subprocess.run([sys.executable, str(SCRIPT)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["next_issue"] == "M276-E001"
    assert payload["dynamic_summary"]["active_runner"]["mode"] == "fast"
    assert "run_m257_a001_lane_a_readiness.log" in payload["dynamic_summary"]["historical_runner"]["log"]
