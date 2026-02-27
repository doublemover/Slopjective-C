#!/usr/bin/env python3
"""Fail-closed readiness check for M01 lane closeout artifacts."""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_M01_DIR = ROOT / "spec" / "planning" / "compiler" / "m01"

REQUIRED_FILES: tuple[str, ...] = (
    "lane_a_m01_closeout_packet_20260226.md",
    "lane_b_m01_closeout_packet_20260226.md",
    "lane_c_m01_closeout_packet_20260226.md",
    "lane_d_m01_closeout_packet_20260226.md",
    "lane_e_m01_closeout_packet_20260226.md",
    "lane_a_m01_issue_closeout_comments_20260226.md",
    "lane_b_m01_issue_closeout_comments_20260226.md",
    "lane_c_m01_issue_closeout_comments_20260226.md",
    "lane_d_m01_issue_closeout_comments_20260226.md",
    "lane_e_m01_issue_closeout_comments_20260226.md",
    "m01_int_rg01_closeout_packet_20260226.md",
    "m01_int_rg01_issue_closeout_comment_20260226.md",
)


def resolve_dir(path: Path) -> Path:
    if path.is_absolute():
        return path
    return ROOT / path


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Check M01 closeout packet/comment readiness."
    )
    parser.add_argument(
        "--m01-dir",
        type=Path,
        default=DEFAULT_M01_DIR,
        help=f"M01 artifact directory (default: {display_path(DEFAULT_M01_DIR)}).",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    m01_dir = resolve_dir(args.m01_dir)

    missing: list[str] = []
    for relative in REQUIRED_FILES:
        candidate = m01_dir / relative
        if not candidate.exists():
            missing.append(display_path(candidate))

    print(f"m01_closeout_dir: {display_path(m01_dir)}")
    print(f"required_files: {len(REQUIRED_FILES)}")
    if missing:
        print(f"status: FAIL (missing={len(missing)})")
        for path in missing:
            print(f"missing: {path}")
        return 1

    print("status: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
