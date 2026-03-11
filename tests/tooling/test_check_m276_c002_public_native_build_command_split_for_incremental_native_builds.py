from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m276_c002_public_native_build_command_split_for_incremental_native_builds.py"
SUMMARY = ROOT / "tmp" / "reports" / "m276" / "M276-C002" / "public_native_build_command_split_summary.json"
CONTRACT_ID = "objc3c-public-native-build-command-split/m276-c002-v1"


def test_m276_c002_checker_emits_summary() -> None:
    completed = subprocess.run([sys.executable, str(SCRIPT)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["next_issue"] == "M276-D001"
    assert payload["dynamic_summary"]["fast_changed_packets"] == []
    assert payload["dynamic_summary"]["contracts_changed_packets"] == [
        "frontend_core_feature_expansion",
        "frontend_invocation_lock",
        "frontend_scaffold",
    ]
    assert payload["dynamic_summary"]["full_changed_packets"] == [
        "frontend_conformance_corpus",
        "frontend_conformance_matrix",
        "frontend_core_feature_expansion",
        "frontend_diagnostics_hardening",
        "frontend_edge_compat",
        "frontend_edge_robustness",
        "frontend_integration_closeout",
        "frontend_invocation_lock",
        "frontend_recovery_determinism_hardening",
        "frontend_scaffold",
    ]
