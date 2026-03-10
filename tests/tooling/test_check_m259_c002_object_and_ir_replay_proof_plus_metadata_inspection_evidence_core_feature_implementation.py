from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m259_c002_object_and_ir_replay_proof_plus_metadata_inspection_evidence_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m259" / "M259-C002" / "object_and_ir_replay_proof_plus_metadata_inspection_summary.json"


def test_checker_passes_in_static_mode(tmp_path: Path) -> None:
    summary_path = tmp_path / "m259_c002_static_summary.json"
    result = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes", "--summary-out", str(summary_path)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )
    assert result.returncode == 0, result.stderr
    payload = json.loads(summary_path.read_text(encoding="utf-8"))
    assert payload["mode"] == "m259-c002-object-ir-replay-proof-and-metadata-inspection-v1"
    assert payload["contract_id"] == "objc3c-runnable-object-ir-replay-and-metadata-inspection/m259-c002-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["next_issue"] == "M259-D001"
    if SUMMARY.exists():
        preserved = json.loads(SUMMARY.read_text(encoding="utf-8"))
        if preserved.get("dynamic_probes_executed"):
            assert "canonical_runnable_replay" in preserved["probe_details"]
