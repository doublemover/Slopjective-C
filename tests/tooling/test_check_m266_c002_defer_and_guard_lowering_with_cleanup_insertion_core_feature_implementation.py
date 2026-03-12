from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m266_c002_defer_and_guard_lowering_with_cleanup_insertion_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m266" / "M266-C002" / "pytest_static_summary.json"


def test_m266_c002_static_bundle_contract_check_runs_truthfully() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes", "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c-part5-defer-guard-lowering-implementation/m266-c002-v1"
    assert payload["surface_path"] == "frontend.pipeline.semantic_surface.objc_part5_control_flow_safety_lowering_contract"
    assert payload["dynamic"]["skipped"] is True
