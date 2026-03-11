from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m261" / "M261-D003" / "block_runtime_byref_forwarding_heap_promotion_ownership_interop_summary.test.json"
CONTRACT_ID = "objc3c-runtime-block-byref-forwarding-heap-promotion-interop/m261-d003-v1"


def test_m261_d003_checker_emits_summary() -> None:
    result = subprocess.run(
        [
            sys.executable,
            str(CHECKER),
            "--skip-dynamic-probes",
            "--summary-out",
            str(SUMMARY),
        ],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["issue"] == "M261-D003"
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["next_issue"] == "M261-E001"
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []
