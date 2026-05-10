from __future__ import annotations

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REPORT_DIR = ROOT / "reports" / "claimability" / "parser-draft-syntax-conformance"
JSON_SUMMARY = REPORT_DIR / "parser_draft_syntax_conformance_summary.json"
MARKDOWN_SUMMARY = REPORT_DIR / "parser_draft_syntax_conformance_summary.md"


def assert_parser_draft_syntax_conformance_artifacts_are_current(
    completed: subprocess.CompletedProcess[str],
) -> None:
    assert completed.returncode == 0, completed.stdout + completed.stderr
    assert JSON_SUMMARY.is_file()
    assert MARKDOWN_SUMMARY.is_file()
