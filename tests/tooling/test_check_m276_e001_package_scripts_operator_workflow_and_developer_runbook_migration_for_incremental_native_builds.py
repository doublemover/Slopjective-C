from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m276_e001_package_scripts_operator_workflow_and_developer_runbook_migration_for_incremental_native_builds.py"
SUMMARY = ROOT / "tmp" / "reports" / "m276" / "M276-E001" / "operator_surface_migration_summary.json"
CONTRACT_ID = "objc3c-incremental-native-build-operator-surface/m276-e001-v1"


def test_m276_e001_checker_emits_summary() -> None:
    completed = subprocess.run([sys.executable, str(SCRIPT)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["next_issue"] == "M276-E002"
    assert "generator" in payload["dynamic_summary"]["fingerprint_keys"]
    assert payload["dynamic_summary"]["compile_commands"].endswith("compile_commands.json")
