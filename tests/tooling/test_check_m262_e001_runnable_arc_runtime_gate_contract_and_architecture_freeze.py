#!/usr/bin/env python3
"""Tooling coverage for M262-E001 runnable ARC runtime gate."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SUMMARY = ROOT / "tmp" / "reports" / "m262" / "M262-E001" / "arc_runtime_gate_summary.json"


def run(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", check=False)


def test_m262_e001_checker_generates_green_summary() -> None:
    completed = run([sys.executable, "scripts/check_m262_e001_runnable_arc_runtime_gate_contract_and_architecture_freeze.py"])
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["contract_id"] == "objc3c-runnable-arc-runtime-gate/m262-e001-v1"
    assert payload["next_issue"] == "M262-E002"
    assert payload["dynamic"]["property_case"]["boundary_line"].startswith(
        "; runnable_arc_runtime_gate = contract=objc3c-runnable-arc-runtime-gate/m262-e001-v1"
    )
    assert payload["dynamic"]["block_case"]["boundary_line"].startswith(
        "; runnable_arc_runtime_gate = contract=objc3c-runnable-arc-runtime-gate/m262-e001-v1"
    )
