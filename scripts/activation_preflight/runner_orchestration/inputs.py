"""Activation preflight input resolution."""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import resolve_repo_path

from scripts.activation_preflight.runner_paths import DEFAULT_ACTIONABLE_STATUSES
from scripts.activation_preflight.runner_state import normalize_actionable_statuses


@dataclass(frozen=True)
class PreflightInputs:
    issues_path: Path
    milestones_path: Path
    catalog_path: Path
    open_blockers_path: Path | None
    output_dir: Path
    t4_overlay_path: Path | None
    expected_actionable_statuses: tuple[str, ...]
    open_blockers_refresh_root: Path | None
    initial_errors: tuple[str, ...]


def _fallback_actionable_statuses(raw_values: Sequence[str] | None) -> tuple[str, ...]:
    statuses = tuple(status for status in (raw_values or []) if status)
    if statuses:
        return statuses
    return DEFAULT_ACTIONABLE_STATUSES


def resolve_preflight_inputs(args: argparse.Namespace) -> PreflightInputs:
    issues_path = resolve_repo_path(args.issues_json)
    milestones_path = resolve_repo_path(args.milestones_json)
    catalog_path = resolve_repo_path(args.catalog_json)
    open_blockers_path = (
        resolve_repo_path(args.open_blockers_json)
        if args.open_blockers_json is not None
        else None
    )
    output_dir = resolve_repo_path(args.output_dir)
    t4_overlay_path = (
        resolve_repo_path(args.t4_governance_overlay_json)
        if args.t4_governance_overlay_json is not None
        else None
    )
    try:
        expected_actionable_statuses = normalize_actionable_statuses(args.actionable_statuses)
    except ValueError as exc:
        expected_actionable_statuses = _fallback_actionable_statuses(args.actionable_statuses)
        initial_errors = (f"invalid actionable-status input: {exc}.",)
    else:
        initial_errors = ()

    open_blockers_refresh_root = (
        resolve_repo_path(args.open_blockers_root) if args.refresh_open_blockers else None
    )

    return PreflightInputs(
        issues_path=issues_path,
        milestones_path=milestones_path,
        catalog_path=catalog_path,
        open_blockers_path=open_blockers_path,
        output_dir=output_dir,
        t4_overlay_path=t4_overlay_path,
        expected_actionable_statuses=expected_actionable_statuses,
        open_blockers_refresh_root=open_blockers_refresh_root,
        initial_errors=initial_errors,
    )


__all__ = ["PreflightInputs", "resolve_preflight_inputs"]
