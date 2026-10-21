from __future__ import annotations

from pathlib import Path

import pytest

from hard_cutover_gate_support import write_fixture
from scripts.source_hygiene.scanner import build_report


@pytest.mark.parametrize(
    "text",
    [
        '"report_path": "tmp/reports/platform-host-evidence/report.json"',
        '"const": "tmp/reports/platform-host-evidence/review-candidate-source-truth.json"',
        'REPORT = ROOT / "tmp/reports/platform-host-evidence/summary.json"',
        'path: tmp/reports/platform-host-evidence/**',
        'Read `tmp/reports/platform-host-evidence/report.json` for diagnostics.',
        'Write the report to `tmp/reports/platform-host-evidence/report.json`.',
        'Evidence is retained under `tmp/reports/platform-host-evidence/`.',
        '| source-of-truth inputs | `tmp/build/report.json` destination |',
    ],
)
def test_report_destinations_are_not_support_claims(tmp_path: Path, text: str) -> None:
    write_fixture(tmp_path / "docs/runbooks/example.md", text + "\n")
    assert build_report(root=tmp_path, scan_roots=("docs",), excludes=())["ok"]


@pytest.mark.parametrize(
    "text",
    [
        'Local-only reports establish support.',
        '`tmp/reports/platform-host-evidence/report.json` is the source of truth.',
        'Use `tmp/reports/platform-host-evidence/report.json` as canonical evidence.',
        '`reports/summary.json` serves as authoritative support.',
        'Support truth comes from `tmp/reports/run/summary.json`.',
        'Closeout evidence is `tmp/reports/run/summary.json`.',
        'The source of truth is `tmp\\reports\\run\\summary.json`.',
        'Confirm public support using `tmp/reports/run/summary.json`.',
        'Promote support from `tmp/reports/run/summary.json`.',
    ],
)
def test_local_reports_cannot_establish_authority(tmp_path: Path, text: str) -> None:
    write_fixture(tmp_path / "docs/runbooks/example.md", text + "\n")
    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())
    assert not report["ok"]
    assert "local-only-report-truth" in {
        finding["pattern_id"] for finding in report["active_findings"]
    }


def test_forbidden_rerouting_is_a_guardrail(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "docs/runbooks/example.md",
        'This action forbids sanitizer/platform fallback routing and cannot promote support.\n',
    )
    assert build_report(root=tmp_path, scan_roots=("docs",), excludes=())["ok"]
