"""Default planning-publication audit paths."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_PAYLOAD = ROOT / "reports" / "planning" / "objc3c_3_next_40_github_payloads.json"
DEFAULT_PUBLICATION_REPORT = ROOT / "reports" / "planning" / "objc3c_3_next_40_publication_report.json"
DEFAULT_MARKDOWN_REPORT = ROOT / "reports" / "planning" / "objc3c_3_next_40_publication_report.md"
DEFAULT_DRIFT_REPORT = ROOT / "reports" / "planning" / "objc3c_3_next_40_publication_drift_report.json"


def repo_path(path: Path) -> Path:
    return path if path.is_absolute() else ROOT / path
