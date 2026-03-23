from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m267_d003_cross_module_error_surface_preservation_hardening_edge_case_and_compatibility_completion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m267" / "M267-D003" / "pytest_cross_module_error_surface_preservation_summary.json"


def test_m267_d003_checker() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT), "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        text=True,
        capture_output=True,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["issue"] == "M267-D003"
    assert payload["contract_id"] == "objc3c-cross-module-error-surface-preservation-hardening/m267-d003-v1"
    assert payload["checks_total"] == payload["checks_passed"]
    dynamic = payload["dynamic"]
    assert dynamic["skipped"] is False
    assert dynamic["happy_path"]["provider_module"] == "m267_d003_cross_module_error_surface_preservation_provider"
    assert dynamic["tampered_path"]["returncode"] != 0
