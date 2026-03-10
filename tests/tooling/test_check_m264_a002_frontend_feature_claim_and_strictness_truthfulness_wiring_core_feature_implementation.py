from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m264_a002_frontend_feature_claim_and_strictness_truthfulness_wiring_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-A002" / "frontend_feature_claim_and_strictness_truthfulness_wiring_summary.json"


def test_checker_passes_in_static_mode(tmp_path: Path) -> None:
    summary_path = tmp_path / "m264_a002_static_summary.json"
    result = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes", "--summary-out", str(summary_path)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )
    assert result.returncode == 0, result.stderr
    payload = json.loads(summary_path.read_text(encoding="utf-8"))
    assert payload["mode"] == "m264-a002-frontend-feature-claim-and-strictness-truth-surface-v1"
    assert payload["contract_id"] == "objc3c-feature-claim-strictness-truth-surface/m264-a002-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["driver_surface_model"] == "language-version-and-compatibility-live-strictness-and-feature-macro-fail-closed"
    if SUMMARY.exists():
        preserved = json.loads(SUMMARY.read_text(encoding="utf-8"))
        assert "dynamic_probes_executed" in preserved
