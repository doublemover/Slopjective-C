from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m272_b002_override_finality_and_sealing_legality_enforcement_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m272" / "M272-B002" / "override_finality_sealing_legality_summary.json"


def test_checker_passes_in_static_mode(tmp_path: Path) -> None:
    summary_path = tmp_path / "m272_b002_static_summary.json"
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
    assert payload["mode"] == "m272-b002-part9-override-finality-sealing-legality-v1"
    assert payload["contract_id"] == "objc3c-part9-override-finality-sealing-legality/m272-b002-v1"
    assert payload["surface_path"] == "frontend.pipeline.semantic_surface.objc_part9_override_finality_and_sealing_legality"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    if SUMMARY.exists():
        preserved = json.loads(SUMMARY.read_text(encoding="utf-8"))
        assert "dynamic_probes_executed" in preserved
