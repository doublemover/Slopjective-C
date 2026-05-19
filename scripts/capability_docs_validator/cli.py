from __future__ import annotations

import argparse
import sys
from typing import Sequence

from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.validation import validate


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Validate canonical capability docs and evidence links.")
    parser.add_argument("--check", action="store_true", help="validate capability docs")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    _ = build_parser().parse_args(argv)
    try:
        validate()
    except CapabilityDocsError as exc:
        print(f"capability docs validation error: {exc}", file=sys.stderr)
        return 1
    print("capability-docs: PASS")
    return 0
