from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m273_e001_metaprogramming_conformance_gate_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m273" / "M273-E001" / "metaprogramming_conformance_gate_summary.json"


def test_m273_e001_checker_writes_static_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c-part10-metaprogramming-conformance-gate/m273-e001-v1"
    assert payload["mode"] == "m273-e001-metaprogramming-conformance-gate-v1"
    assert payload["next_closeout_issue"] == "M273-E002"
    assert payload["dynamic"]["skipped"] is True
    assert payload["upstream"]["M273-D002"]["contract_id"] == "objc3c-part10-macro-host-process-cache-runtime-integration/m273-d002-v1"
    assert payload["ok"] is True


def test_m273_e001_checker_writes_dynamic_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["dynamic"]["skipped"] is False
    assert payload["upstream"]["M273-D002"]["dynamic_probes_executed"] is True
    assert payload["ok"] is True
