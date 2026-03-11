from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = (
    ROOT
    / "scripts"
    / "check_m260_d002_reference_counting_weak_table_and_autoreleasepool_implementation_core_feature_implementation.py"
)
SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m260"
    / "M260-D002"
    / "reference_counting_weak_autoreleasepool_summary.json"
)


def run_checker(*args: str) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(CHECKER), *args],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def test_m260_d002_checker_static_contract() -> None:
    result = run_checker("--skip-dynamic-probes")
    assert result.returncode == 0, result.stdout + result.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["dynamic"]["skipped"] is True


def test_m260_d002_checker_packet_mentions_next_issue() -> None:
    packet = (
        ROOT
        / "spec"
        / "planning"
        / "compiler"
        / "m260"
        / "m260_d002_reference_counting_weak_table_and_autoreleasepool_implementation_core_feature_implementation_packet.md"
    ).read_text(encoding="utf-8")
    assert "`M260-E001`" in packet
