from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m261" / "M261-C004" / "escaping_block_runtime_hook_lowering_summary.test.json"
CONTRACT_ID = "objc3c-executable-block-escape-runtime-hook-lowering/m261-c004-v1"


def test_m261_c004_checker_emits_summary() -> None:
    subprocess.run(
        [
            sys.executable,
            str(CHECKER),
            "--skip-dynamic-probes",
            "--summary-out",
            str(SUMMARY),
        ],
        cwd=ROOT,
        check=True,
    )
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["next_issue"] == "M261-D001"
    assert payload["dynamic_probes_executed"] is False
