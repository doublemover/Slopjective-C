#!/usr/bin/env python3

from __future__ import annotations

import argparse
import sys
from collections.abc import Callable
from pathlib import Path

import check_planning_placeholders
import check_planning_unchecked_checkboxes
import spec_lint

DEFAULT_INCLUDE_GLOBS = (
    "spec/planning/active/**/*.md",
)
DEFAULT_EXCLUDE_GLOBS = (
    "spec/planning/archive/**",
    "spec/planning/generated/**",
)
DEFAULT_UNCHECKED_CHECKBOX_EXCLUDE_GLOBS = (
    *DEFAULT_EXCLUDE_GLOBS,
)
DEFAULT_PLACEHOLDER_ARGS = ("--format", "json")


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Run structural lint checks against planning markdown files."
    )
    parser.add_argument(
        "--glob",
        action="append",
        default=[],
        dest="include_globs",
        help=(
            "Include glob relative to repository root (repeatable). "
            "If omitted, planning defaults are used."
        ),
    )
    parser.add_argument(
        "--exclude",
        action="append",
        default=[],
        dest="exclude_globs",
        help=(
            "Exclude glob relative to repository root (repeatable). "
            "If omitted, planning defaults are used."
        ),
    )
    return parser.parse_args(argv)


def normalize_globs(
    values: list[str], defaults: tuple[str, ...]
) -> tuple[str, ...]:
    source = values if values else list(defaults)
    return tuple(raw_glob.replace("\\", "/") for raw_glob in source)


def resolve_scope(
    include_globs: tuple[str, ...], exclude_globs: tuple[str, ...]
) -> list[Path]:
    included_paths = spec_lint.iter_spec_files_for_globs(include_globs)
    excluded_paths: set[Path] = set()

    for exclude_glob in exclude_globs:
        for path in spec_lint.ROOT.glob(exclude_glob):
            if path.is_file():
                excluded_paths.add(path.resolve())

    return [
        path for path in included_paths if path.resolve() not in excluded_paths
    ]


def resolve_anchor_scope(lint_paths: list[Path]) -> list[Path]:
    # Reuse the broader spec anchor universe so planning files can reference
    # canonical anchors outside `spec/planning/**` without false positives.
    anchor_paths = spec_lint.iter_spec_files_for_globs(spec_lint.DEFAULT_INCLUDE_GLOBS)
    merged: dict[Path, Path] = {}
    for path in anchor_paths + lint_paths:
        resolved = path.resolve()
        merged[resolved] = resolved
    return sorted(merged.values(), key=lambda path: path.relative_to(spec_lint.ROOT).as_posix())


def run_structural_lint(paths: list[Path]) -> int:
    all_errors: list[spec_lint.LintError] = []
    anchor_scope_paths = resolve_anchor_scope(paths)
    global_anchors, file_anchors = spec_lint.collect_anchor_index(anchor_scope_paths)

    for path in paths:
        all_errors.extend(
            spec_lint.lint_file(
                path,
                global_anchors=global_anchors,
                file_anchors=file_anchors,
            )
        )

    if all_errors:
        for err in all_errors:
            rel = err.path.relative_to(spec_lint.ROOT)
            print(f"{rel}:{err.line}: {err.message}")
        print(f"\nFound {len(all_errors)} spec-lint issue(s).")
        return 1

    print("spec-lint: OK")
    return 0


def run_placeholders_lint() -> int:
    return check_planning_placeholders.main(list(DEFAULT_PLACEHOLDER_ARGS))


def run_unchecked_checkboxes_lint(
    include_globs: tuple[str, ...],
    exclude_globs: tuple[str, ...],
) -> int:
    args: list[str] = ["--format", "json"]
    for include_glob in include_globs:
        args.extend(["--glob", include_glob])
    for exclude_glob in exclude_globs:
        args.extend(["--exclude", exclude_glob])
    return check_planning_unchecked_checkboxes.main(args)


def run_check(name: str, callback: Callable[[], int]) -> int:
    try:
        return callback()
    except Exception as exc:  # pragma: no cover - defensive failure mapping
        print(
            f"planning-lint: {name} raised an unexpected exception: {exc}",
            file=sys.stderr,
        )
        return 2


def aggregate_exit_codes(*, structural: int, placeholders: int, unchecked: int) -> int:
    codes = (structural, placeholders, unchecked)
    if any(code >= 2 for code in codes):
        return 2
    if any(code == 1 for code in codes):
        return 1
    return 0


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    include_globs = normalize_globs(args.include_globs, DEFAULT_INCLUDE_GLOBS)
    exclude_globs = normalize_globs(args.exclude_globs, DEFAULT_EXCLUDE_GLOBS)
    unchecked_exclude_globs = normalize_globs(
        args.exclude_globs,
        DEFAULT_UNCHECKED_CHECKBOX_EXCLUDE_GLOBS,
    )
    scoped_paths = resolve_scope(include_globs, exclude_globs)

    if not scoped_paths:
        include_list = ", ".join(include_globs)
        exclude_list = ", ".join(exclude_globs) if exclude_globs else "(none)"
        print("planning-lint: no files matched include/exclude scope", file=sys.stderr)
        print(f"planning-lint: include globs -> {include_list}", file=sys.stderr)
        print(f"planning-lint: exclude globs -> {exclude_list}", file=sys.stderr)
        return 2

    structural_code = run_check("structural spec lint", lambda: run_structural_lint(scoped_paths))
    placeholder_code = run_check("planning placeholder lint", run_placeholders_lint)
    unchecked_code = run_check(
        "planning unchecked checkbox lint",
        lambda: run_unchecked_checkboxes_lint(
            include_globs,
            unchecked_exclude_globs,
        ),
    )

    print(
        "planning-lint summary: "
        f"structural={structural_code} "
        f"placeholders={placeholder_code} "
        f"unchecked_checkboxes={unchecked_code}"
    )

    return aggregate_exit_codes(
        structural=structural_code,
        placeholders=placeholder_code,
        unchecked=unchecked_code,
    )


if __name__ == "__main__":
    sys.exit(main())
