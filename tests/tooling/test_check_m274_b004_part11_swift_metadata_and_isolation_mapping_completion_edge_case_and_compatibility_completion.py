from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m274_b004_part11_swift_metadata_and_isolation_mapping_completion_edge_case_and_compatibility_completion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m274" / "M274-B004" / "part11_swift_metadata_and_isolation_mapping_summary.test.json"
CONTRACT_ID = "objc3c-part11-swift-metadata-and-isolation-mapping-completion/m274-b004-v1"


def test_m274_b004_checker_emits_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes", "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr

    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["surface_path"] == "frontend.pipeline.semantic_surface.objc_part11_swift_metadata_and_isolation_mapping"
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["dynamic_probes_executed"] is False

    dynamic_summary = payload["dynamic_summary"]
    assert dynamic_summary["skipped"] is True
