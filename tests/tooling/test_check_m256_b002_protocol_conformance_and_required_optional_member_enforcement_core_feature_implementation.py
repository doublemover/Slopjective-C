from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = (
    ROOT
    / "scripts"
    / "check_m256_b002_protocol_conformance_and_required_optional_member_enforcement_core_feature_implementation.py"
)
SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m256"
    / "M256-B002"
    / "protocol_conformance_required_optional_member_enforcement_summary.json"
)


def test_checker_passes() -> None:
    result = subprocess.run(
        [sys.executable, str(SCRIPT)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert result.returncode == 0, result.stderr or result.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["mode"] == "m256-b002-protocol-conformance-required-optional-core-feature-implementation-v1"
    assert payload["contract_id"] == "objc3c-protocol-conformance-required-optional-enforcement/m256-b002-v1"
    assert payload["previous_contract_id"] == "objc3c-object-model-semantic-rules/m256-b001-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes"]["positive"]["returncode"] == 0
    assert payload["dynamic_probes"]["missing_required_method"]["returncode"] != 0
    assert payload["dynamic_probes"]["incompatible_method"]["returncode"] != 0
    assert payload["dynamic_probes"]["missing_required_property"]["returncode"] != 0
    assert payload["dynamic_probes"]["incompatible_property"]["returncode"] != 0


def test_checker_fails_closed_when_packet_contract_id_drifts(tmp_path: Path) -> None:
    source_packet = (
        ROOT
        / "spec"
        / "planning"
        / "compiler"
        / "m256"
        / "m256_b002_protocol_conformance_and_required_optional_member_enforcement_core_feature_implementation_packet.md"
    )
    drift_packet = tmp_path / "packet.md"
    drift_packet.write_text(
        source_packet.read_text(encoding="utf-8").replace(
            "`objc3c-protocol-conformance-required-optional-enforcement/m256-b002-v1`",
            "`objc3c-protocol-conformance-required-optional-enforcement/m256-b002-drift`",
            1,
        ),
        encoding="utf-8",
    )
    summary_out = tmp_path / "summary.json"
    completed = subprocess.run(
        [
            sys.executable,
            str(SCRIPT),
            "--packet-doc",
            str(drift_packet),
            "--summary-out",
            str(summary_out),
            "--skip-dynamic-probes",
        ],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert completed.returncode == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M256-B002-DOC-PKT-05" for failure in payload["failures"])
