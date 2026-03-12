from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m262_d003_ownership_debug_instrumentation_and_runtime_validation_hooks_for_arc_core_feature_expansion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m262" / "M262-D003" / "arc_debug_instrumentation_summary.json"


def test_checker_static_mode_passes() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c-runtime-arc-debug-instrumentation/m262-d003-v1"
    assert payload["dynamic_probes_executed"] is False
    assert payload["failures"] == []
