"""Command-line flow for Objective-C 3.0 library/CLI parity checks."""

from __future__ import annotations

import argparse
import sys
from typing import Sequence

from objc3c_library_cli_parity.arguments import parse_args as _parse_args
from objc3c_library_cli_parity.contracts import DEFAULT_ARTIFACTS, MODE
from objc3c_library_cli_parity.orchestration import run_from_args as _run_from_args


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    return _parse_args(argv)


def run(argv: Sequence[str]) -> int:
    return _run_from_args(parse_args(argv))


def main() -> None:
    raise SystemExit(run(sys.argv[1:]))


__all__ = ["DEFAULT_ARTIFACTS", "MODE", "main", "parse_args", "run"]
