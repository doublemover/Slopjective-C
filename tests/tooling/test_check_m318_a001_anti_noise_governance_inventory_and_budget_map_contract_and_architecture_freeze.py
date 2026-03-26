from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m318_a001_anti_noise_governance_inventory_and_budget_map_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m318" / "M318-A001" / "anti_noise_governance_inventory_summary.json"


def test_m318_a001_checker_passes_and_writes_summary() -> None:
    result = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert result.returncode == 0, result.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["public_command_budget"]["maximum_total_public_entrypoints"] == 25
    assert payload["public_command_budget"]["current_public_entrypoints"] == 17
    assert payload["validation_growth_budget"]["closeout_maximums"]["check_scripts"] == 558
    assert payload["source_hygiene_budget"]["post_cleanup_native_source_milestone_token_lines"] == 57
    assert payload["artifact_authenticity_budget"]["synthetic_stub_ir_files"] == 2
    assert payload["exception_process_transition"]["active_exception_count"] == 0
