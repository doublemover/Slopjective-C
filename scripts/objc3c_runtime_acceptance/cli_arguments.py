"""CLI argument model for runtime acceptance."""

from __future__ import annotations

import argparse

from objc3c_runtime_acceptance.suite_catalog import RUNTIME_ACCEPTANCE_SUITE_CASES


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Run ObjC3 runtime acceptance suites."
    )
    parser.add_argument(
        "--suite",
        choices=sorted(RUNTIME_ACCEPTANCE_SUITE_CASES),
        default="full",
        help="named runtime acceptance suite to run",
    )
    parser.add_argument(
        "--case",
        action="append",
        default=[],
        dest="cases",
        help="run one case label; may be repeated and overrides --suite",
    )
    parser.add_argument(
        "--list-suites",
        action="store_true",
        help="print suite names and case labels without running acceptance",
    )
    return parser.parse_args(argv)


__all__ = ["parse_args"]
