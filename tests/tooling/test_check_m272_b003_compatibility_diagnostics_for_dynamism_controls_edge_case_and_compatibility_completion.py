from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m272_b003_compatibility_diagnostics_for_dynamism_controls_edge_case_and_compatibility_completion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m272" / "M272-B003" / "dispatch_control_compatibility_summary.json"


def test_checker_passes_in_static_mode(tmp_path: Path) -> None:
    summary_path = tmp_path / "m272_b003_static_summary.json"
    result = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes", "--summary-out", str(summary_path)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert result.returncode == 0, result.stderr or result.stdout
    payload = json.loads(summary_path.read_text(encoding="utf-8"))
    assert payload["mode"] == "m272-b003-part9-dispatch-control-compatibility-v1"
    assert payload["contract_id"] == "objc3c-part9-dynamism-control-compatibility-diagnostics/m272-b003-v1"
    assert payload["surface_path"] == "frontend.pipeline.semantic_surface.objc_part9_dynamism_control_compatibility_diagnostics"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    if SUMMARY.exists():
        preserved = json.loads(SUMMARY.read_text(encoding="utf-8"))
        assert "dynamic_probes_executed" in preserved
