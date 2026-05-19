"""CLI for release/runtime claim matrix publication."""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path

from .paths import JSON_OUT, MD_OUT
from .publish import publish_matrix


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Publish the release/runtime claim matrix.")
    parser.add_argument("--json-out", type=Path, default=JSON_OUT)
    parser.add_argument("--md-out", type=Path, default=MD_OUT)
    return parser.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    publish_matrix(args.json_out, args.md_out)
    print(f"[ok] wrote {display_path(args.json_out)}")
    print(f"[ok] wrote {display_path(args.md_out)}")
    return 0
