from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m259_d001_toolchain_and_runtime_operations_contract_and_architecture_freeze.py"


def test_checker_passes() -> None:
    summary_path = ROOT / "tmp" / "reports" / "m259" / "M259-D001" / "pytest_toolchain_and_runtime_operations_contract_summary.json"
    result = subprocess.run(
        [sys.executable, str(SCRIPT), "--summary-out", str(summary_path)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )
    assert result.returncode == 0, result.stderr
    payload = json.loads(summary_path.read_text(encoding="utf-8"))
    assert payload["mode"] == "m259-d001-toolchain-and-runtime-operations-freeze-v1"
    assert payload["contract_id"] == "objc3c-runnable-toolchain-runtime-operations-freeze/m259-d001-v1"
    assert payload["next_issue"] == "M259-D002"
    assert payload["ok"] is True
