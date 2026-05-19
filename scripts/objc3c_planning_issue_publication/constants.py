"""Constants for Objective-C 3 planning issue publication."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_PAYLOAD = ROOT / "reports" / "planning" / "objc3c_3_next_40_github_payloads.json"
DEFAULT_REPORT = ROOT / "reports" / "planning" / "objc3c_3_next_40_publication_report.json"

DESCRIPTION = """Publish checked-in Objective-C 3 planning issues to GitHub.

The publisher intentionally treats GitHub numbers as assigned state. Draft IDs
stay stable, and the durable report records the GitHub milestone/issue numbers
that were actually returned by the API.
"""
