from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m264_b002_fail_closed_unsupported_feature_claim_enforcement_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m264" / "M264-B002" / "fail_closed_unsupported_feature_claim_enforcement_summary.json"


def test_checker_passes_in_static_mode(tmp_path: Path) -> None:
    summary_path = tmp_path / "m264_b002_static_summary.json"
    result = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes", "--summary-out", str(summary_path)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )
    assert result.returncode == 0, result.stderr
    payload = json.loads(summary_path.read_text(encoding="utf-8"))
    assert payload["mode"] == "m264-b002-fail-closed-unsupported-feature-claim-enforcement-v1"
    assert payload["contract_id"] == "objc3c-fail-closed-unsupported-feature-claim-enforcement/m264-b002-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["surface_path"] == "frontend.pipeline.semantic_surface.objc_compatibility_strictness_claim_semantics"
    if SUMMARY.exists():
        preserved = json.loads(SUMMARY.read_text(encoding="utf-8"))
        assert "negative_cases" in preserved["probe_details"]
