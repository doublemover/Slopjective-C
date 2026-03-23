from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m268_b003_async_diagnostics_and_compatibility_completion_edge_case_and_compatibility_completion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m268" / "M268-B003" / "async_diagnostics_compatibility_completion_summary.json"


def test_checker_passes_in_static_mode(tmp_path: Path) -> None:
    summary_path = tmp_path / "m268_b003_static_summary.json"
    result = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes", "--summary-out", str(summary_path)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert result.returncode == 0, result.stderr
    payload = json.loads(summary_path.read_text(encoding="utf-8"))
    assert payload["mode"] == "m268-b003-part7-async-diagnostics-compatibility-v1"
    assert payload["contract_id"] == "objc3c-part7-async-diagnostics-compatibility-completion/m268-b003-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    if SUMMARY.exists():
        preserved = json.loads(SUMMARY.read_text(encoding="utf-8"))
        assert "dynamic_probes_executed" in preserved
