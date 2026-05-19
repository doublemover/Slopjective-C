"""CLI argument parsing facade for governance sustainability evidence contracts."""

from __future__ import annotations

import argparse
from collections.abc import Sequence

from .contracts import CONTRACT_ID


def build_argument_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Inspect governance sustainability evidence contract metadata."
    )
    parser.add_argument(
        "--contract-id",
        action="store_true",
        help="Print the governance sustainability evidence contract id.",
    )
    return parser


def parse_arguments(argv: Sequence[str] | None = None) -> argparse.Namespace:
    return build_argument_parser().parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_arguments(argv)
    if args.contract_id:
        print(CONTRACT_ID)
    return 0


__all__ = [
    "build_argument_parser",
    "main",
    "parse_arguments",
]
