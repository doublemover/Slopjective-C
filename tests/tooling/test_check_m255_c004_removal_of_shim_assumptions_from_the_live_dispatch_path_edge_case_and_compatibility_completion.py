from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m255_c004_removal_of_shim_assumptions_from_the_live_dispatch_path_edge_case_and_compatibility_completion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m255" / "M255-C004" / "live_dispatch_cutover_summary.json"


def test_m255_c004_checker() -> None:
    result = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    summary = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert summary["ok"] is True
    assert summary["dynamic_probes"]["nil"]["canonical_call_count"] == 1
    assert summary["dynamic_probes"]["nil"]["compatibility_call_count"] == 0
    assert summary["dynamic_probes"]["dynamic"]["canonical_call_count"] == 1
    assert summary["dynamic_probes"]["dynamic"]["compatibility_call_count"] == 0
    assert summary["dynamic_probes"]["dynamic"]["program_exit_code"] == summary["dynamic_probes"]["dynamic"]["expected_exit_code"]
    assert summary["dynamic_probes"]["super_dynamic"]["canonical_call_count"] == 7
    assert summary["dynamic_probes"]["super_dynamic"]["compatibility_call_count"] == 0
