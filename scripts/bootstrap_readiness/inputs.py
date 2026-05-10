"""Bootstrap readiness path defaults and input resolution."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from objc3c_tooling.paths import resolve_repo_path

from .constants import DEFAULT_REFRESH_OPEN_BLOCKERS_RELATIVE_PATH


@dataclass(frozen=True)
class ResolvedInputs:
    issues_path: Path
    milestones_path: Path
    catalog_path: Path
    output_dir: Path
    open_blockers_path: Path | None
    refresh_open_blockers_root: Path | None


def default_open_blockers_output_path(output_dir: Path) -> Path:
    return output_dir / DEFAULT_REFRESH_OPEN_BLOCKERS_RELATIVE_PATH


def resolve_inputs(args: object) -> ResolvedInputs:
    issues_path = resolve_repo_path(args.issues_json)
    milestones_path = resolve_repo_path(args.milestones_json)
    catalog_path = resolve_repo_path(args.catalog_json)
    output_dir = resolve_repo_path(args.output_dir)
    open_blockers_path = (
        resolve_repo_path(args.open_blockers_json)
        if args.open_blockers_json is not None
        else None
    )
    refresh_open_blockers_root = (
        resolve_repo_path(args.open_blockers_root)
        if args.refresh_open_blockers
        else None
    )
    return ResolvedInputs(
        issues_path=issues_path,
        milestones_path=milestones_path,
        catalog_path=catalog_path,
        output_dir=output_dir,
        open_blockers_path=open_blockers_path,
        refresh_open_blockers_root=refresh_open_blockers_root,
    )
