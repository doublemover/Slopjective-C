#!/usr/bin/env python3
"""Fail-closed readiness check for M13 lane execution artifacts."""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_M13_DIR = ROOT / "spec" / "planning" / "compiler" / "m13"

REQUIRED_FILES: tuple[str, ...] = (
    "m13_dispatch_snapshot_20260226.json",
    "m13_dispatch_snapshot_20260226.md",
    "m13_parallel_dispatch_plan_20260226.md",
    "lane_a_m13_scope_freeze.md",
    "lane_a_m13_execution_packet_20260226.md",
    "lane_a_m13_issue_comment_templates_20260226.md",
    "lane_b_m13_scope_freeze.md",
    "lane_b_m13_execution_packet_20260226.md",
    "lane_b_m13_issue_comment_templates_20260226.md",
    "lane_c_m13_scope_freeze.md",
    "lane_c_m13_execution_packet_20260226.md",
    "lane_c_m13_issue_comment_templates_20260226.md",
    "lane_d_m13_scope_freeze.md",
    "lane_d_m13_execution_packet_20260226.md",
    "lane_d_m13_issue_comment_templates_20260226.md",
    "lane_e_m13_scope_freeze.md",
    "lane_e_m13_execution_packet_20260226.md",
    "lane_e_m13_issue_comment_templates_20260226.md",
    "m13_int_rg01_scope_freeze.md",
    "m13_int_rg01_execution_packet_20260226.md",
    "m13_int_rg01_issue_comment_templates_20260226.md",
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
    parser = argparse.ArgumentParser(description="Check M13 execution packet readiness.")
    parser.add_argument(
        "--m13-dir",
        type=Path,
        default=DEFAULT_M13_DIR,
        help=f"M13 artifact directory (default: {display_path(DEFAULT_M13_DIR)}).",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    m13_dir = resolve_dir(args.m13_dir)

    missing: list[str] = []
    for relative in REQUIRED_FILES:
        candidate = m13_dir / relative
        if not candidate.exists():
            missing.append(display_path(candidate))

    print(f"m13_execution_dir: {display_path(m13_dir)}")
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



