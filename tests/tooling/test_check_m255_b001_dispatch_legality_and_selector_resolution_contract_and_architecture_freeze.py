from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = (
    ROOT
    / "scripts"
    / "check_m255_b001_dispatch_legality_and_selector_resolution_contract_and_architecture_freeze.py"
)
SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m255"
    / "M255-B001"
    / "dispatch_legality_selector_resolution_contract_summary.json"
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
    assert payload["mode"] == "m255-b001-dispatch-legality-selector-resolution-freeze-v1"
    assert (
        payload["contract_id"]
        == "objc3c-dispatch-legality-selector-resolution/m255-b001-v1"
    )
    assert payload["ok"] is True
    assert (
        payload["boundary_model"]
        == "selector-normalized-arity-checked-receiver-required-no-overload"
    )
    assert (
        payload["ambiguity_policy"]
        == "fail-closed-on-unresolved-or-ambiguous-selector-resolution"
    )
    assert payload["supported_selector_forms"] == "unary-and-keyword-selectors"
    assert payload["next_issue"] == "M255-B002"
