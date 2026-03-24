from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SUMMARY = ROOT / "tmp" / "reports" / "m271" / "M271-D001" / "system_helper_runtime_contract_summary.json"


def test_m271_d001_checker_passes() -> None:
    completed = subprocess.run(
        [
            sys.executable,
            "scripts/check_m271_d001_system_helper_and_runtime_contract_and_architecture_freeze.py",
            "--skip-dynamic-probes",
            "--summary-out",
            str(SUMMARY),
        ],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["mode"] == "m271-d001-system-helper-runtime-contract-freeze-v1"
    assert payload["contract_id"] == "objc3c-part8-system-helper-runtime-contract/m271-d001-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
