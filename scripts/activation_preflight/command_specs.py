"""Activation preflight command-spec builders."""

from __future__ import annotations

from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path

from scripts.activation_preflight.contracts import CommandSpec


def snapshot_refresh_spec(
    *,
    script_path: Path,
    issues_path: Path,
    milestones_path: Path,
    generated_at_utc: str | None,
) -> CommandSpec:
    actual_args: list[str] = [
        "--issues-output",
        str(issues_path),
        "--milestones-output",
        str(milestones_path),
    ]
    display_args: list[str] = [
        "--issues-output",
        display_path(issues_path),
        "--milestones-output",
        display_path(milestones_path),
    ]
    if generated_at_utc is not None:
        actual_args.extend(["--generated-at-utc", generated_at_utc])
        display_args.extend(["--generated-at-utc", generated_at_utc])

    return CommandSpec(
        name="capture_activation_snapshots",
        script_path=script_path,
        actual_args=tuple(actual_args),
        display_args=tuple(display_args),
    )


def open_blockers_refresh_spec(
    *,
    script_path: Path,
    root: Path,
    generated_at_utc: str | None,
    source: str | None,
) -> CommandSpec:
    actual_args: list[str] = [
        "--root",
        str(root),
        "--format",
        "snapshot-json",
    ]
    display_args: list[str] = [
        "--root",
        display_path(root),
        "--format",
        "snapshot-json",
    ]
    if generated_at_utc is not None:
        actual_args.extend(["--generated-at-utc", generated_at_utc])
        display_args.extend(["--generated-at-utc", generated_at_utc])
    if source is not None:
        actual_args.extend(["--source", source])
        display_args.extend(["--source", source])

    return CommandSpec(
        name="extract_open_blockers_snapshot_json",
        script_path=script_path,
        actual_args=tuple(actual_args),
        display_args=tuple(display_args),
    )


def activation_check_specs(
    *,
    script_path: Path,
    issues_path: Path,
    milestones_path: Path,
    catalog_path: Path,
    open_blockers_path: Path | None,
    actionable_statuses: Sequence[str] | None,
    issues_max_age_seconds: int | None,
    milestones_max_age_seconds: int | None,
    t4_overlay_path: Path | None,
    t4_new_scope_publish: bool,
) -> tuple[CommandSpec, CommandSpec]:
    actual_args: list[str] = [
        "--issues-json",
        str(issues_path),
        "--milestones-json",
        str(milestones_path),
        "--catalog-json",
        str(catalog_path),
    ]
    display_args: list[str] = [
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

    for status in actionable_statuses or ():
        actual_args.extend(["--actionable-status", status])
        display_args.extend(["--actionable-status", status])

    if issues_max_age_seconds is not None:
        actual_args.extend(["--issues-max-age-seconds", str(issues_max_age_seconds)])
        display_args.extend(["--issues-max-age-seconds", str(issues_max_age_seconds)])
    if milestones_max_age_seconds is not None:
        actual_args.extend(["--milestones-max-age-seconds", str(milestones_max_age_seconds)])
        display_args.extend(["--milestones-max-age-seconds", str(milestones_max_age_seconds)])

    if t4_overlay_path is not None:
        actual_args.extend(["--t4-governance-overlay-json", str(t4_overlay_path)])
        display_args.extend(["--t4-governance-overlay-json", display_path(t4_overlay_path)])
    elif t4_new_scope_publish:
        actual_args.append("--t4-new-scope-publish")
        display_args.append("--t4-new-scope-publish")

    json_spec = CommandSpec(
        name="check_activation_triggers_json",
        script_path=script_path,
        actual_args=tuple([*actual_args, "--format", "json"]),
        display_args=tuple([*display_args, "--format", "json"]),
    )
    markdown_spec = CommandSpec(
        name="check_activation_triggers_markdown",
        script_path=script_path,
        actual_args=tuple([*actual_args, "--format", "markdown"]),
        display_args=tuple([*display_args, "--format", "markdown"]),
    )
    return json_spec, markdown_spec


def spec_lint_spec(*, script_path: Path, spec_globs: Sequence[str]) -> CommandSpec:
    actual_args: list[str] = []
    display_args: list[str] = []
    for glob in spec_globs:
        actual_args.extend(["--glob", glob])
        display_args.extend(["--glob", glob])

    return CommandSpec(
        name="spec_lint",
        script_path=script_path,
        actual_args=tuple(actual_args),
        display_args=tuple(display_args),
    )


__all__ = [
    "activation_check_specs",
    "open_blockers_refresh_spec",
    "snapshot_refresh_spec",
    "spec_lint_spec",
]
