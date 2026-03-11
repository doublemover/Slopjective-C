from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m276_c003_frontend_packet_dependency_shape_decomposition_for_incremental_build_split.py"
SUMMARY = ROOT / "tmp" / "reports" / "m276" / "M276-C003" / "frontend_packet_dependency_shape_decomposition_summary.json"
CONTRACT_ID = "objc3c-frontend-packet-dependency-shape-decomposition/m276-c003-v1"


def test_m276_c003_checker_emits_summary() -> None:
    completed = subprocess.run([sys.executable, str(SCRIPT)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["next_issue"] == "M276-C002"
    family_map = payload["dynamic_summary"]["family_map"]
    assert family_map["source-derived"] == ["frontend_scaffold"]
    assert family_map["binary-derived"] == ["frontend_invocation_lock", "frontend_core_feature_expansion"]
    assert "frontend_integration_closeout" in family_map["closeout-derived"]
    assert payload["dynamic_summary"]["packets_source"]["changed"] == ["frontend_scaffold"]
    assert payload["dynamic_summary"]["packets_binary"]["changed"] == [
        "frontend_core_feature_expansion",
        "frontend_invocation_lock",
        "frontend_scaffold",
    ]
    assert payload["dynamic_summary"]["packets_closeout"]["changed"] == [
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
