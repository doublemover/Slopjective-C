#!/usr/bin/env python3
"""Refresh compiler dispatch snapshot JSON and Markdown artifacts."""

from __future__ import annotations

import argparse
import importlib.util
import sys
from pathlib import Path
from types import ModuleType
from typing import Sequence

def load_dispatch_plan_module() -> ModuleType:
    try:
        import generate_compiler_dispatch_plan as dispatch_module

        return dispatch_module
    except ModuleNotFoundError:
        module_path = Path(__file__).resolve().parent / "generate_compiler_dispatch_plan.py"
        spec = importlib.util.spec_from_file_location(
            "generate_compiler_dispatch_plan", module_path
        )
        if spec is None or spec.loader is None:
            raise RuntimeError("Unable to load scripts/generate_compiler_dispatch_plan.py")
        dispatch_module = importlib.util.module_from_spec(spec)
        sys.modules[spec.name] = dispatch_module
        spec.loader.exec_module(dispatch_module)
        return dispatch_module


dispatch_plan = load_dispatch_plan_module()


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description=(
            "Refresh deterministic compiler dispatch snapshot artifacts "
            "(JSON + Markdown) from an issues snapshot."
        )
    )
    parser.add_argument(
        "--issues-json",
        type=Path,
        required=True,
        help="Path to the GitHub issues snapshot JSON input.",
    )
    parser.add_argument(
        "--milestone-number",
        type=int,
        required=True,
        help="Milestone number to snapshot.",
    )
    parser.add_argument(
        "--top-n",
        type=int,
        required=True,
        help="Number of next tasks to emit per lane.",
    )
    parser.add_argument(
        "--output-json",
        type=Path,
        required=True,
        help="Destination path for snapshot JSON output.",
    )
    parser.add_argument(
        "--output-md",
        type=Path,
        required=True,
        help="Destination path for snapshot Markdown output.",
    )
    return parser


def write_text(path: Path, content: str) -> None:
    try:
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content, encoding="utf-8")
    except OSError as exc:
        raise ValueError(f"unable to write output file {dispatch_plan.display_path(path)}: {exc}") from exc


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    if args.top_n <= 0:
        print("error: --top-n must be > 0", file=sys.stderr)
        return 2

    issues_json_path = dispatch_plan.normalize_path(args.issues_json)
    output_json_path = dispatch_plan.normalize_path(args.output_json)
    output_md_path = dispatch_plan.normalize_path(args.output_md)

    try:
        rows = dispatch_plan.parse_issue_rows(issues_json_path)
        milestone_number = dispatch_plan.pick_target_milestone(rows, args.milestone_number)
        payload = dispatch_plan.build_payload(
            rows,
            milestone_number=milestone_number,
            top_n=args.top_n,
        )
        payload["source"]["issues_json"] = dispatch_plan.display_path(issues_json_path)
        json_output = dispatch_plan.render_json(payload)
        markdown_output = dispatch_plan.render_markdown(payload)

        write_text(output_json_path, json_output)
        write_text(output_md_path, markdown_output)
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
