from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SUMMARY = (
    ROOT
    / "reports"
    / "claimability"
    / "parser-container-layout"
    / "parser_container_layout_summary.json"
)


def assert_parser_container_layout_summary_artifact_exists() -> None:
    assert SUMMARY.is_file()
