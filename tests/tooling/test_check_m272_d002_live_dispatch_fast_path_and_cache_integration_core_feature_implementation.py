from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m272_d002_live_dispatch_fast_path_and_cache_integration_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m272" / "M272-D002" / "live_dispatch_fast_path_summary.json"


def test_checker_passes_in_static_mode(tmp_path: Path) -> None:
    summary_path = tmp_path / "m272_d002_static_summary.json"
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
    assert payload["mode"] == "m272-d002-part9-live-dispatch-fast-path-and-cache-integration-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    if SUMMARY.exists():
        preserved = json.loads(SUMMARY.read_text(encoding="utf-8"))
        assert "dynamic_probes_executed" in preserved
