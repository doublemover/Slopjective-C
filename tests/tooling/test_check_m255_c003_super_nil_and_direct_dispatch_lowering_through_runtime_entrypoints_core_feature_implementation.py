from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = (
    ROOT
    / "scripts"
    / "check_m255_c003_super_nil_and_direct_dispatch_lowering_through_runtime_entrypoints_core_feature_implementation.py"
)
SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m255"
    / "M255-C003"
    / "super_nil_direct_runtime_dispatch_summary.json"
)


def test_m255_c003_checker_passes() -> None:
    result = subprocess.run([sys.executable, str(SCRIPT)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert result.returncode == 0, result.stdout + result.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    nil_case = payload["dynamic_probes"]["nil"]
    assert nil_case["canonical_call_count"] == 1
    assert nil_case["compatibility_call_count"] == 0
    assert nil_case["program_exit_code"] == nil_case["expected_exit_code"] == 9
    super_case = payload["dynamic_probes"]["super_dynamic"]
    assert super_case["canonical_call_count"] == 4
    assert super_case["compatibility_call_count"] == 3
