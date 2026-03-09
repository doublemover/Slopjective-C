from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m255_e002_replace_shim_based_execution_smoke_with_live_dispatch_evidence_cross_lane_integration_sync.py"
SUMMARY = ROOT / "tmp" / "reports" / "m255" / "M255-E002" / "live_dispatch_smoke_replay_closeout_summary.json"


def test_m255_e002_checker_passes_static_contract() -> None:
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
    assert SUMMARY.exists()
    summary_text = SUMMARY.read_text(encoding="utf-8")
    assert '"contract_id": "objc3c-runtime-live-dispatch-smoke-replay-closeout/m255-e002-v1"' in summary_text
    assert '"ok": true' in summary_text
