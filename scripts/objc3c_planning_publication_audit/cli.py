"""Command-line parsing for the planning-publication drift audit."""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import Sequence

from .paths import DEFAULT_DRIFT_REPORT, DEFAULT_MARKDOWN_REPORT, DEFAULT_PAYLOAD, DEFAULT_PUBLICATION_REPORT


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Audit Objective-C 3 planning publication drift and refresh durable references.")
    parser.add_argument("--payload", type=Path, default=DEFAULT_PAYLOAD)
    parser.add_argument("--publication-report", type=Path, default=DEFAULT_PUBLICATION_REPORT)
    parser.add_argument("--markdown-report", type=Path, default=DEFAULT_MARKDOWN_REPORT)
    parser.add_argument("--output", type=Path, default=DEFAULT_DRIFT_REPORT)
    parser.add_argument("--write", action="store_true", help="Write the drift report.")
    parser.add_argument("--check", action="store_true", help="Fail if checked-in generated references are out of sync.")
    parser.add_argument("--update-payload-published", action="store_true", help="Replace payload.published from the JSON publication report.")
    parser.add_argument("--update-markdown-report", action="store_true", help="Rewrite the markdown publication report from JSON.")
    parser.add_argument("--live", action="store_true", help="Compare the durable mapping against live GitHub issues and milestones.")
    parser.add_argument("--limit-live", type=int, default=None, help="Limit live issue checks; milestones are always fully checked.")
    return parser.parse_args(argv)
