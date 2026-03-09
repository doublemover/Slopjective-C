from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = (
    ROOT
    / "scripts"
    / "check_m255_c002_runtime_call_abi_generation_for_instance_and_class_sends_core_feature_implementation.py"
)
SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m255"
    / "M255-C002"
    / "runtime_call_abi_generation_summary.json"
)


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
    assert payload["mode"] == "m255-c002-runtime-call-abi-generation-for-instance-and-class-sends-core-feature-implementation-v1"
    assert payload["contract_id"] == "objc3c-runtime-call-abi-instance-class-dispatch/m255-c002-v1"
    assert payload["ok"] is True
    positive = payload["dynamic_probes"]["positive"]
    assert positive["returncode"] == 0
    assert positive["canonical_call_count"] == 2
    assert positive["compatibility_call_count"] == 0
    assert positive["program_exit_code"] == positive["expected_exit_code"]
    deferred = payload["dynamic_probes"]["deferred"]
    assert deferred["returncode"] == 0
    assert deferred["compatibility_call_count"] >= 1
    assert deferred["canonical_call_count"] == 0
