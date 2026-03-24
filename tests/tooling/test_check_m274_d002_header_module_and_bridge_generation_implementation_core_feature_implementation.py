from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SUMMARY = ROOT / "tmp" / "reports" / "m274" / "M274-D002" / "header_module_bridge_generation_summary.json"
CHECKER = ROOT / "scripts" / "check_m274_d002_header_module_and_bridge_generation_implementation_core_feature_implementation.py"


def run_checker(*args: str) -> None:
    completed = subprocess.run([sys.executable, str(CHECKER), *args], cwd=ROOT, check=False)
    assert completed.returncode == 0


def test_checker_passes_static() -> None:
    run_checker("--skip-dynamic-probes")
    summary = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert summary["ok"] is True
    assert summary["dynamic_probes"]["skipped"] is True


def test_checker_passes_dynamic() -> None:
    run_checker()
    summary = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert summary["ok"] is True
    assert summary["dynamic_probes"]["runtime_probe"]["bridge_generation_ready"] == "1"
    assert summary["dynamic_probes"]["consumer_link_plan"]["part11_header_module_bridge_cross_module_packaging_ready"] is True

