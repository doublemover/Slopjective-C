from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m255_a001_dispatch_surface_classification_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m255" / "M255-A001" / "dispatch_surface_classification_contract_summary.json"


def test_checker_passes() -> None:
    result = subprocess.run(
        [sys.executable, str(SCRIPT)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )
    assert result.returncode == 0, result.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["mode"] == "m255-a001-dispatch-surface-classification-freeze-v1"
    assert payload["contract_id"] == "objc3c-dispatch-surface-classification/m255-a001-v1"
    assert payload["ok"] is True
    assert payload["live_runtime_entrypoint_family"] == "objc3_runtime_dispatch_i32-objc3_msgsend_i32-compat"
    assert payload["direct_dispatch_binding"] == "reserved-non-goal"
    assert payload["next_issue"] == "M255-A002"
