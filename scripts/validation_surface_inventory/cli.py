"""Command-line interface for the validation surface inventory builder."""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import Sequence

from .paths import JSON_OUT, MD_OUT, REPORT_DIR
from .public import write_report


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Build the validation surface inventory report.")
    parser.add_argument("--report-dir", type=Path, default=REPORT_DIR)
    parser.add_argument("--json-out", type=Path)
    parser.add_argument("--md-out", type=Path)
    return parser


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = build_parser()
    args, _unknown = parser.parse_known_args(argv)
    args.json_out = args.json_out or args.report_dir / JSON_OUT.name
    args.md_out = args.md_out or args.report_dir / MD_OUT.name
    return args


def run(args: argparse.Namespace) -> int:
    write_report(report_dir=args.report_dir, json_out=args.json_out, md_out=args.md_out)
    return 0


def main(argv: Sequence[str] | None = None) -> int:
    return run(parse_args(argv))
