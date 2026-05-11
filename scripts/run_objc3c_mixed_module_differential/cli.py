"""CLI parsing for the mixed-module differential runner."""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import Sequence

from .config import MANIFEST_PATH, SUMMARY_PATH


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Run mixed-module and import/export differential stress validation.")
    parser.add_argument("--manifest", type=Path, default=MANIFEST_PATH)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--contract-mode", action="store_true")
    return parser


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    return build_arg_parser().parse_args(argv)


__all__ = ["build_arg_parser", "parse_args"]
