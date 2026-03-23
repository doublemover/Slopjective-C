from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m271_b001_system_extension_semantic_model_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m271" / "M271-B001" / "system_extension_semantic_model_summary.json"


def test_checker_passes_in_static_mode(tmp_path: Path) -> None:
    summary_path = tmp_path / "m271_b001_static_summary.json"
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
    assert payload["mode"] == "m271-b001-part8-system-extension-semantic-model-v1"
    assert payload["contract_id"] == "objc3c-part8-system-extension-semantic-model/m271-b001-v1"
    assert payload["surface_path"] == "frontend.pipeline.semantic_surface.objc_part8_system_extension_semantic_model"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    if SUMMARY.exists():
        preserved = json.loads(SUMMARY.read_text(encoding="utf-8"))
        assert "dynamic_probes_executed" in preserved
