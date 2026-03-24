from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m275_a001_diagnostics_fix_it_and_migrator_source_surface_inventory_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m275" / "M275-A001" / "diagnostics_fixit_migrator_source_inventory_summary.json"


def test_m275_a001_checker_writes_summary() -> None:
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
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c-part12-diagnostics-fixit-migrator-source-inventory/m275-a001-v1"
    assert payload["mode"] == "m275-a001-part12-diagnostics-fixit-migrator-source-inventory-v1"
    assert payload["next_issue"] == "M275-A002"
    assert payload["ok"] is True


def test_m275_a001_checker_writes_dynamic_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    part12 = payload["dynamic_summary"]["part12_surface"]
    assert part12["advanced_feature_family_count"] == 6
    assert part12["dependency_surface_count"] == 14
    assert part12["diagnostic_surface_sites"] > 0
