"""Command spec construction for bootstrap readiness orchestration."""

from __future__ import annotations

from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path

from .constants import (
    CHECK_BOOTSTRAP_READINESS_SCRIPT_PATH,
    EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH,
    SPEC_LINT_SCRIPT_PATH,
)
from .models import CommandSpec


def open_blockers_refresh_spec(
    *,
    root: Path,
    generated_at_utc: str,
    source: str,
) -> CommandSpec:
    actual_args = [
        "--root",
        str(root),
        "--format",
        "snapshot-json",
        "--generated-at-utc",
        generated_at_utc,
        "--source",
        source,
    ]
    display_args = [
        "--root",
        display_path(root),
        "--format",
        "snapshot-json",
        "--generated-at-utc",
        generated_at_utc,
        "--source",
        source,
    ]
    return CommandSpec(
        name="extract_open_blockers_snapshot_json",
        script_path=EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH,
        actual_args=tuple(actual_args),
        display_args=tuple(display_args),
    )


def checker_specs(
    *,
    issues_path: Path,
    milestones_path: Path,
    catalog_path: Path,
    open_blockers_path: Path | None,
) -> tuple[CommandSpec, CommandSpec]:
    actual_args = [
        "--issues-json",
        str(issues_path),
        "--milestones-json",
        str(milestones_path),
        "--catalog-json",
        str(catalog_path),
    ]
    display_args = [
        "--issues-json",
        display_path(issues_path),
        "--milestones-json",
        display_path(milestones_path),
        "--catalog-json",
        display_path(catalog_path),
    ]
    if open_blockers_path is not None:
        actual_args.extend(["--open-blockers-json", str(open_blockers_path)])
        display_args.extend(["--open-blockers-json", display_path(open_blockers_path)])

    return (
        CommandSpec(
            name="check_bootstrap_readiness_json",
            script_path=CHECK_BOOTSTRAP_READINESS_SCRIPT_PATH,
            actual_args=tuple([*actual_args, "--format", "json"]),
            display_args=tuple([*display_args, "--format", "json"]),
        ),
        CommandSpec(
            name="check_bootstrap_readiness_markdown",
            script_path=CHECK_BOOTSTRAP_READINESS_SCRIPT_PATH,
            actual_args=tuple([*actual_args, "--format", "md"]),
            display_args=tuple([*display_args, "--format", "md"]),
        ),
    )


def spec_lint_spec(spec_globs: Sequence[str]) -> CommandSpec:
    actual_args: list[str] = []
    display_args: list[str] = []
    for glob in spec_globs:
        actual_args.extend(["--glob", glob])
        display_args.extend(["--glob", glob])

    return CommandSpec(
        name="spec_lint",
        script_path=SPEC_LINT_SCRIPT_PATH,
        actual_args=tuple(actual_args),
        display_args=tuple(display_args),
    )
