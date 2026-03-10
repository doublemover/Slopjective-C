from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m259_b001_runnable_core_compatibility_guard.py"
SUMMARY = ROOT / "tmp" / "reports" / "m259" / "M259-B001" / "runnable_core_compatibility_guard_summary.json"
CONTRACT_ID = "objc3c-runnable-core-compatibility-guard/m259-b001-v1"


def test_checker_passes(tmp_path: Path) -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT)],
        cwd=str(ROOT),
        check=False,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr


def test_summary_contains_expected_guard_boundary() -> None:
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["next_issue"] == "M259-B002"
    assert payload["dependency"]["M259-A002"]["contract_id"] == "objc3c-canonical-runnable-sample-set/m259-a002-v1"
    assert payload["dependency"]["M259-A002"]["ok"] is True
    assert "O3S216" in payload["guard_boundary"]["landed_fail_closed_diagnostics"]
    assert "throws" in payload["guard_boundary"]["non_runnable_advanced_claim_families"]
    assert "async-await" in payload["guard_boundary"]["non_runnable_advanced_claim_families"]
