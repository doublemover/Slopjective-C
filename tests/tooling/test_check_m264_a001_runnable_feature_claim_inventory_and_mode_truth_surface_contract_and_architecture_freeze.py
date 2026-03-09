from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m264_a001_runnable_feature_claim_inventory_and_mode_truth_surface_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-A001" / "runnable_feature_claim_inventory_and_mode_truth_surface_summary.json"


def test_checker_passes_in_static_mode(tmp_path: Path) -> None:
    summary_path = tmp_path / "m264_a001_static_summary.json"
    result = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes", "--summary-out", str(summary_path)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )
    assert result.returncode == 0, result.stderr
    payload = json.loads(summary_path.read_text(encoding="utf-8"))
    assert payload["mode"] == "m264-a001-runnable-feature-claim-inventory-v1"
    assert payload["contract_id"] == "objc3c-runnable-feature-claim-inventory/m264-a001-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["truth_model"] == "truthful-runnable-subset-plus-source-only-plus-fail-closed-unsupported"
    if SUMMARY.exists():
        preserved = json.loads(SUMMARY.read_text(encoding="utf-8"))
        assert "dynamic_probes_executed" in preserved
