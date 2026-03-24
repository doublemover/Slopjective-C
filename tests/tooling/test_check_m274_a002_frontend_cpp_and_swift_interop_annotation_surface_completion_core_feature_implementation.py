from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m274_a002_frontend_cpp_and_swift_interop_annotation_surface_completion_core_feature_implementation.py"
TEST_SUMMARY = ROOT / "tmp" / "reports" / "m274" / "M274-A002" / "cpp_swift_interop_annotation_source_completion_summary.test.json"
CONTRACT_ID = "objc3c-part11-cpp-swift-interop-annotation-source-completion/m274-a002-v1"


def test_m274_a002_checker_emits_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT), "--summary-out", str(TEST_SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(TEST_SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["dynamic_probes_executed"] is True
    packet = payload["dynamic_summary"]["part11_cpp_and_swift_interop_annotation_source_completion_packet"]
    assert packet["swift_name_annotation_sites"] == 1
    assert packet["swift_private_annotation_sites"] == 1
    assert packet["cpp_name_annotation_sites"] == 1
    assert packet["header_name_annotation_sites"] == 1
    assert packet["interop_metadata_annotation_sites"] == 4
    assert packet["named_annotation_payload_sites"] == 3
    assert packet["swift_annotation_source_supported"] is True
    assert packet["cpp_annotation_source_supported"] is True
    assert packet["interop_metadata_source_supported"] is True
    assert packet["deterministic_handoff"] is True
    assert packet["ready_for_semantic_expansion"] is True

