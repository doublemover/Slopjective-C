"""CLI parsing for the objc3c stress-minimization runner."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Sequence

from .paths import ARTIFACT_SURFACE_PATH, COMPILER, MANIFEST_PATH, SUMMARY_PATH


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Reduce failing checked-in stress seeds into machine-owned replay capsules."
    )
    parser.add_argument("--compiler", type=Path, default=COMPILER)
    parser.add_argument("--manifest", type=Path, default=MANIFEST_PATH)
    parser.add_argument("--artifact-surface", type=Path, default=ARTIFACT_SURFACE_PATH)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--timeout-sec", type=float, default=5.0)
    parser.add_argument("--contract-mode", action="store_true")
    return parser


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    return build_arg_parser().parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    from .runner import run_stress_minimization

    args = parse_args(argv or sys.argv[1:])
    return run_stress_minimization(args)


__all__ = ["build_arg_parser", "main", "parse_args"]
