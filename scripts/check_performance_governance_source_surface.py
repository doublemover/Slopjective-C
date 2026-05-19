#!/usr/bin/env python3
"""Validate the checked-in performance-governance source surface."""

from __future__ import annotations

import sys
from pathlib import Path

_ROOT = Path(__file__).resolve().parents[1]
_SCRIPT_ROOT = _ROOT / "scripts"
for _import_root in (_ROOT, _SCRIPT_ROOT):
    _import_root_text = str(_import_root)
    if _import_root_text not in sys.path:
        sys.path.insert(0, _import_root_text)

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_tooling.paths import repo_rel

from scripts.check_performance_governance_source_surface.config import SourceSurfaceConfig
from scripts.check_performance_governance_source_surface.constants import (
    EXPECTED_BUILD_SCRIPTS,
    EXPECTED_CHECKED_IN_ROOTS,
    EXPECTED_CHECKED_IN_SOURCES,
    EXPECTED_EXPLICIT_NON_GOALS,
    EXPECTED_MACHINE_OWNED_OUTPUT_ROOTS,
    EXPECTED_OWNER_SPLIT,
    EXPECTED_REQUIRED_PATHS,
    EXPECTED_RUNBOOK,
    EXPECTED_UPSTREAM_REPORTS,
    ROOT,
    SOURCE_SURFACE,
    SOURCE_SURFACE_CONTRACT_ID,
    SOURCE_SURFACE_KIND,
    SUMMARY_CONTRACT_ID,
    SUMMARY_PATH,
)
from scripts.check_performance_governance_source_surface.runner import run
from scripts.check_performance_governance_source_surface.validation import fail as _fail
from scripts.check_performance_governance_source_surface.validation import (
    require_exact_list as _require_exact_list,
)
from scripts.check_performance_governance_source_surface.validation import (
    require_exact_owner_split as _require_exact_owner_split,
)
from scripts.check_performance_governance_source_surface.validation import (
    require_exact_path as _require_exact_path,
)
from scripts.check_performance_governance_source_surface.validation import (
    require_path as _require_path,
)


def fail(message: str) -> int:
    return _fail(message)


def require_exact_path(source_surface: dict[str, object], field_name: str) -> str | None:
    return _require_exact_path(
        source_surface,
        field_name,
        expected_required_paths=EXPECTED_REQUIRED_PATHS,
        fail_handler=fail,
    )


def require_path(relative_path: str, *, kind: str) -> bool:
    return _require_path(relative_path, kind=kind, root=ROOT, fail_handler=fail)


def require_exact_list(
    source_surface: dict[str, object],
    field_name: str,
    expected_items: tuple[str, ...],
) -> tuple[str, ...] | None:
    return _require_exact_list(
        source_surface,
        field_name,
        expected_items,
        fail_handler=fail,
    )


def require_exact_owner_split(source_surface: dict[str, object]) -> dict[str, list[str]] | None:
    return _require_exact_owner_split(
        source_surface,
        expected_owner_split=EXPECTED_OWNER_SPLIT,
        fail_handler=fail,
    )


def _current_config() -> SourceSurfaceConfig:
    return SourceSurfaceConfig(
        root=ROOT,
        source_surface=SOURCE_SURFACE,
        summary_path=SUMMARY_PATH,
        source_surface_contract_id=SOURCE_SURFACE_CONTRACT_ID,
        source_surface_kind=SOURCE_SURFACE_KIND,
        summary_contract_id=SUMMARY_CONTRACT_ID,
        expected_runbook=EXPECTED_RUNBOOK,
        expected_required_paths=EXPECTED_REQUIRED_PATHS,
        expected_upstream_reports=EXPECTED_UPSTREAM_REPORTS,
        expected_checked_in_sources=EXPECTED_CHECKED_IN_SOURCES,
        expected_build_scripts=EXPECTED_BUILD_SCRIPTS,
        expected_owner_split=EXPECTED_OWNER_SPLIT,
        expected_machine_owned_output_roots=EXPECTED_MACHINE_OWNED_OUTPUT_ROOTS,
        expected_explicit_non_goals=EXPECTED_EXPLICIT_NON_GOALS,
        expected_checked_in_roots=EXPECTED_CHECKED_IN_ROOTS,
    )


def main() -> int:
    return run(_current_config(), fail_handler=fail)


__all__ = [
    "EXPECTED_BUILD_SCRIPTS",
    "EXPECTED_CHECKED_IN_ROOTS",
    "EXPECTED_CHECKED_IN_SOURCES",
    "EXPECTED_EXPLICIT_NON_GOALS",
    "EXPECTED_MACHINE_OWNED_OUTPUT_ROOTS",
    "EXPECTED_OWNER_SPLIT",
    "EXPECTED_REQUIRED_PATHS",
    "EXPECTED_RUNBOOK",
    "EXPECTED_UPSTREAM_REPORTS",
    "Path",
    "ROOT",
    "SOURCE_SURFACE",
    "SOURCE_SURFACE_CONTRACT_ID",
    "SOURCE_SURFACE_KIND",
    "SUMMARY_CONTRACT_ID",
    "SUMMARY_PATH",
    "SourceSurfaceConfig",
    "fail",
    "load_json",
    "main",
    "repo_rel",
    "require_exact_list",
    "require_exact_owner_split",
    "require_exact_path",
    "require_path",
    "run",
    "sys",
    "write_report_json",
]


if __name__ == "__main__":
    raise SystemExit(main())
