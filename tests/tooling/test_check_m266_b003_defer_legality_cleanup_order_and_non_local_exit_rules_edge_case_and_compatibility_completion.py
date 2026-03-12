from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m266_b003_defer_legality_cleanup_order_and_non_local_exit_rules_edge_case_and_compatibility_completion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m266" / "M266-B003" / "pytest_static_summary.json"


def test_m266_b003_static_contract_check_passes() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes", "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["checks_failed"] == 0
    assert payload["contract_id"] == "objc3c-part5-control-flow-semantic-model/m266-b001-v1"
