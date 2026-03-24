from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m273_d002_macro_host_process_and_toolchain_integration_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m273" / "M273-D002" / "macro_host_process_cache_runtime_integration_summary.json"


def test_checker_passes_static() -> None:
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
    summary = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert summary["ok"] is True
    assert summary["dynamic_probes_executed"] is False
    assert summary["contract_id"] == "objc3c-part10-macro-host-process-cache-runtime-integration/m273-d002-v1"


def test_checker_passes_dynamic() -> None:
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
    assert SUMMARY.exists()
    summary = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert summary["ok"] is True
    assert summary["dynamic_probes_executed"] is True
    assert summary["implementation_ready"] is True
    assert summary["contract_id"] == "objc3c-part10-macro-host-process-cache-runtime-integration/m273-d002-v1"
