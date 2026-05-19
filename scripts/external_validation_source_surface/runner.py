"""CLI runner for the external validation source-surface checker."""

from __future__ import annotations

import sys
from pathlib import Path

from .paths import EXPECTED_ROOTS, SOURCE_SURFACE, SUMMARY_PATH
from .publication import print_success, publish_summary
from .rendering import render_summary
from .validation import ValidationFailure, validate_source_surface


def fail(message: str) -> int:
    print(f"external-validation-source-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def run(
    *,
    source_surface_path: Path,
    summary_path: Path,
    expected_roots: tuple[str, ...] = EXPECTED_ROOTS,
) -> int:
    try:
        validation = validate_source_surface(source_surface_path, expected_roots=expected_roots)
    except ValidationFailure as exc:
        return fail(str(exc))

    summary = render_summary(validation, source_surface_path=source_surface_path)
    publish_summary(summary_path, summary)
    print_success(summary_path)
    return 0


def main() -> int:
    return run(
        source_surface_path=SOURCE_SURFACE,
        summary_path=SUMMARY_PATH,
        expected_roots=EXPECTED_ROOTS,
    )


if __name__ == "__main__":
    raise SystemExit(main())
