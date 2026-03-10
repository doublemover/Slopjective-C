from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m259_c001_end_to_end_replay_and_inspection_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m259" / "M259-C001" / "end_to_end_replay_and_inspection_summary.json"


def test_checker_passes_in_static_mode(tmp_path: Path) -> None:
    summary_path = tmp_path / "m259_c001_static_summary.json"
    result = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes", "--summary-out", str(summary_path)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )
    assert result.returncode == 0, result.stderr
    payload = json.loads(summary_path.read_text(encoding="utf-8"))
    assert payload["mode"] == "m259-c001-end-to-end-replay-and-inspection-freeze-v1"
    assert payload["contract_id"] == "objc3c-runnable-replay-and-inspection-evidence-freeze/m259-c001-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["next_issue"] == "M259-C002"
    if SUMMARY.exists():
        preserved = json.loads(SUMMARY.read_text(encoding="utf-8"))
        assert "binary_inspection" in preserved["probe_details"]
