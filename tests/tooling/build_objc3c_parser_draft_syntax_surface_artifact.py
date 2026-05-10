from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SUMMARY = (
    ROOT
    / "reports"
    / "claimability"
    / "parser-draft-syntax-surface"
    / "parser_draft_syntax_surface_summary.json"
)


def assert_parser_draft_syntax_surface_summary_artifact_exists() -> None:
    assert SUMMARY.is_file()
