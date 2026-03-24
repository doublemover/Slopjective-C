from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SUMMARY = ROOT / "tmp" / "reports" / "m274" / "M274-D001" / "bridge_packaging_toolchain_contract_summary.json"
CHECKER = ROOT / "scripts" / "check_m274_d001_bridge_packaging_and_toolchain_contract_and_architecture_freeze.py"


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
    assert summary["dynamic_probes"]["runtime_probe"]["packaging_topology_ready"] == "1"
    assert summary["dynamic_probes"]["consumer_link_plan"]["part11_ffi_cross_module_packaging_ready"] is True
