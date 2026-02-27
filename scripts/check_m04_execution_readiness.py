#!/usr/bin/env python3
"""Fail-closed readiness check for M04 lane execution artifacts."""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_M04_DIR = ROOT / "spec" / "planning" / "compiler" / "m04"

REQUIRED_FILES: tuple[str, ...] = (
    "m04_dispatch_snapshot_20260226.json",
    "m04_dispatch_snapshot_20260226.md",
    "m04_parallel_dispatch_plan_20260226.md",
    "lane_a_m04_scope_freeze.md",
    "lane_a_m04_execution_packet_20260226.md",
    "lane_a_m04_issue_comment_templates_20260226.md",
    "lane_b_m04_scope_freeze.md",
    "lane_b_m04_execution_packet_20260226.md",
    "lane_b_m04_issue_comment_templates_20260226.md",
    "lane_c_m04_scope_freeze.md",
    "lane_c_m04_execution_packet_20260226.md",
    "lane_c_m04_issue_comment_templates_20260226.md",
    "lane_d_m04_scope_freeze.md",
    "lane_d_m04_execution_packet_20260226.md",
    "lane_d_m04_issue_comment_templates_20260226.md",
    "lane_e_m04_scope_freeze.md",
    "lane_e_m04_execution_packet_20260226.md",
    "lane_e_m04_issue_comment_templates_20260226.md",
    "m04_int_rg01_scope_freeze.md",
    "m04_int_rg01_execution_packet_20260226.md",
    "m04_int_rg01_issue_comment_templates_20260226.md",
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
    parser = argparse.ArgumentParser(description="Check M04 execution packet readiness.")
    parser.add_argument(
        "--m04-dir",
        type=Path,
        default=DEFAULT_M04_DIR,
        help=f"M04 artifact directory (default: {display_path(DEFAULT_M04_DIR)}).",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    m04_dir = resolve_dir(args.m04_dir)

    missing: list[str] = []
    for relative in REQUIRED_FILES:
        candidate = m04_dir / relative
        if not candidate.exists():
            missing.append(display_path(candidate))

    print(f"m04_execution_dir: {display_path(m04_dir)}")
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
