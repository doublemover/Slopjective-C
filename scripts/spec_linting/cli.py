from __future__ import annotations

import argparse

from .checks import collect_anchor_index, lint_file
from .discovery import iter_spec_files, iter_spec_files_for_globs
from .models import LintError
from .reporting import render_failure_report, render_success_report


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Run structural lint checks against spec markdown files."
    )
    parser.add_argument(
        "--glob",
        action="append",
        default=[],
        dest="include_globs",
        help=(
            "Include glob relative to repository root (repeatable). "
            "If omitted, repository defaults are used."
        ),
    )
    return parser


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    return build_parser().parse_args(argv)


def run_lint(include_globs: list[str]) -> list[LintError]:
    if include_globs:
        spec_files = iter_spec_files_for_globs(include_globs)
    else:
        spec_files = iter_spec_files()

    global_anchors, file_anchors = collect_anchor_index(spec_files)
    errors: list[LintError] = []
    for path in spec_files:
        errors.extend(
            lint_file(
                path,
                global_anchors=global_anchors,
                file_anchors=file_anchors,
            )
        )
    return errors


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    errors = run_lint(args.include_globs)

    if errors:
        print(render_failure_report(errors))
        return 1

    print(render_success_report())
    return 0
