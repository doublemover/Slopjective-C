"""Open-blocker audit scope resolution and extractor command specs."""

from __future__ import annotations

from dataclasses import replace
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path
from scripts.open_blocker_extraction.audit_scope import (
    DEFAULT_EXCLUDE_PATHS,
    build_extractor_exclude_paths,
    normalize_exclude_paths,
    normalize_include_globs,
    resolve_effective_audit_root,
    resolve_markdown_scope,
)

from .constants import EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH
from .models import AuditScope, CommandSpec


def prepare_audit_scope(
    *,
    audit_root: Path,
    raw_include_globs: Sequence[str],
    raw_exclude_paths: Sequence[str],
    include_default_excludes: bool,
) -> tuple[AuditScope, list[str]]:
    errors: list[str] = []
    effective_audit_root = audit_root

    try:
        exclude_paths = normalize_exclude_paths(
            raw_exclude_paths,
            include_defaults=include_default_excludes,
        )
    except ValueError as exc:
        exclude_paths = ()
        errors.append(str(exc))

    try:
        include_globs = normalize_include_globs(raw_include_globs)
    except ValueError as exc:
        include_globs = ()
        errors.append(str(exc))

    if not errors:
        try:
            effective_audit_root = resolve_effective_audit_root(
                audit_root=audit_root,
                include_globs=include_globs,
            )
        except ValueError as exc:
            errors.append(str(exc))

    return (
        AuditScope(
            audit_root=audit_root,
            effective_audit_root=effective_audit_root,
            include_globs=include_globs,
            exclude_paths=exclude_paths,
            included_markdown_paths=(),
            excluded_markdown_paths=(),
            extractor_exclude_paths=(),
        ),
        errors,
    )


def resolve_scope_markdown_paths(scope: AuditScope) -> tuple[AuditScope, list[str]]:
    errors: list[str] = []
    included_markdown_paths: list[str] = []
    excluded_markdown_paths: list[str] = []
    extractor_exclude_paths: tuple[str, ...] = ()

    try:
        included_markdown_paths, excluded_markdown_paths = resolve_markdown_scope(
            audit_root=scope.effective_audit_root,
            exclude_paths=scope.exclude_paths,
        )
    except ValueError as exc:
        errors.append(str(exc))

    if not errors and not included_markdown_paths:
        errors.append(
            "no markdown files matched audit scope after exclusions under "
            f"{display_path(scope.effective_audit_root)}."
        )

    if not errors:
        extractor_exclude_paths = build_extractor_exclude_paths(
            exclude_paths=scope.exclude_paths,
            markdown_paths=[*included_markdown_paths, *excluded_markdown_paths],
        )

    return (
        replace(
            scope,
            included_markdown_paths=tuple(included_markdown_paths),
            excluded_markdown_paths=tuple(excluded_markdown_paths),
            extractor_exclude_paths=extractor_exclude_paths,
        ),
        errors,
    )


def resolve_audit_scope(
    *,
    audit_root: Path,
    raw_include_globs: Sequence[str],
    raw_exclude_paths: Sequence[str],
    include_default_excludes: bool,
) -> tuple[AuditScope, list[str]]:
    scope, errors = prepare_audit_scope(
        audit_root=audit_root,
        raw_include_globs=raw_include_globs,
        raw_exclude_paths=raw_exclude_paths,
        include_default_excludes=include_default_excludes,
    )
    if not errors:
        scope, markdown_errors = resolve_scope_markdown_paths(scope)
        errors.extend(markdown_errors)
    return scope, errors


def build_extract_snapshot_spec(
    *,
    effective_audit_root: Path,
    generated_at_utc: str,
    source: str,
    extractor_exclude_paths: Sequence[str],
) -> CommandSpec:
    actual_args: list[str] = [
        "--root",
        str(effective_audit_root),
        "--format",
        "snapshot-json",
        "--generated-at-utc",
        generated_at_utc,
        "--source",
        source,
    ]
    display_args: list[str] = [
        "--root",
        display_path(effective_audit_root),
        "--format",
        "snapshot-json",
        "--generated-at-utc",
        generated_at_utc,
        "--source",
        source,
    ]
    for exclude_path in extractor_exclude_paths:
        actual_args.extend(["--exclude-path", exclude_path])
        display_args.extend(["--exclude-path", exclude_path])

    return CommandSpec(
        name="extract_open_blockers_snapshot_json",
        script_path=EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH,
        actual_args=tuple(actual_args),
        display_args=tuple(display_args),
    )


__all__ = [
    "DEFAULT_EXCLUDE_PATHS",
    "build_extract_snapshot_spec",
    "build_extractor_exclude_paths",
    "normalize_exclude_paths",
    "normalize_include_globs",
    "prepare_audit_scope",
    "resolve_audit_scope",
    "resolve_effective_audit_root",
    "resolve_markdown_scope",
    "resolve_scope_markdown_paths",
]
