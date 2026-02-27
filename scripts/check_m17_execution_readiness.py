#!/usr/bin/env python3
"""Fail-closed readiness check for M17 lane execution artifacts."""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_M17_DIR = ROOT / "spec" / "planning" / "compiler" / "m17"

REQUIRED_FILES: tuple[str, ...] = (
    "m17_dispatch_snapshot_20260226.json",
    "m17_dispatch_snapshot_20260226.md",
    "m17_parallel_dispatch_plan_20260226.md",
    "lane_a_m17_scope_freeze.md",
    "lane_a_m17_execution_packet_20260226.md",
    "lane_a_m17_issue_comment_templates_20260226.md",
    "lane_b_m17_scope_freeze.md",
    "lane_b_m17_execution_packet_20260226.md",
    "lane_b_m17_issue_comment_templates_20260226.md",
    "lane_c_m17_scope_freeze.md",
    "lane_c_m17_execution_packet_20260226.md",
    "lane_c_m17_issue_comment_templates_20260226.md",
    "lane_d_m17_scope_freeze.md",
    "lane_d_m17_execution_packet_20260226.md",
    "lane_d_m17_issue_comment_templates_20260226.md",
    "lane_e_m17_scope_freeze.md",
    "lane_e_m17_execution_packet_20260226.md",
    "lane_e_m17_issue_comment_templates_20260226.md",
    "m17_int_rg01_scope_freeze.md",
    "m17_int_rg01_execution_packet_20260226.md",
    "m17_int_rg01_issue_comment_templates_20260226.md",
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
    parser = argparse.ArgumentParser(description="Check M17 execution packet readiness.")
    parser.add_argument(
        "--m17-dir",
        type=Path,
        default=DEFAULT_M17_DIR,
        help=f"M17 artifact directory (default: {display_path(DEFAULT_M17_DIR)}).",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    m17_dir = resolve_dir(args.m17_dir)

    missing: list[str] = []
    for relative in REQUIRED_FILES:
        candidate = m17_dir / relative
        if not candidate.exists():
            missing.append(display_path(candidate))

    print(f"m17_execution_dir: {display_path(m17_dir)}")
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



