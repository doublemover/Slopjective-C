"""Command-line parsing for the behavior matrix checker."""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import Sequence

from objc3c_tooling.behavior_fixtures import REQUIRED_TREE

from .config import REPORT_PATH


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run the behavior-first native fixture matrix from tests/native.")
    parser.add_argument("--phase", choices=sorted(REQUIRED_TREE), action="append", default=[])
    parser.add_argument("--limit", type=int, default=0)
    parser.add_argument("--report-out", type=Path, default=REPORT_PATH)
    return parser.parse_args(argv)
