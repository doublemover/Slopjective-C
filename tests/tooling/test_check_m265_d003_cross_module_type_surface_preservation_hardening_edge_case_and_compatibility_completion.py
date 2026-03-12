from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m265_d003_cross_module_type_surface_preservation_hardening_edge_case_and_compatibility_completion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m265" / "M265-D003" / "cross_module_type_surface_preservation_summary.json"


def test_m265_d003_static_checker(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    result = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes", "--summary-out", str(summary_out)],
        cwd=ROOT,
        check=False,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
    )
    assert result.returncode == 0, result.stdout + result.stderr
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["issue"] == "M265-D003"
    assert payload["checks_passed"] == payload["checks_total"]


def test_m265_d003_summary_target_path_is_stable() -> None:
    assert SUMMARY.as_posix().endswith("tmp/reports/m265/M265-D003/cross_module_type_surface_preservation_summary.json")
