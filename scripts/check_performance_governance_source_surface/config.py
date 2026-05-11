"""Configuration model for the source-surface checker."""

from __future__ import annotations

from collections.abc import Mapping, Sequence
from dataclasses import dataclass
from pathlib import Path

from .constants import (
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


@dataclass(frozen=True)
class SourceSurfaceConfig:
    root: Path
    source_surface: Path
    summary_path: Path
    source_surface_contract_id: str
    source_surface_kind: str
    summary_contract_id: str
    expected_runbook: str
    expected_required_paths: Mapping[str, str]
    expected_upstream_reports: Sequence[str]
    expected_checked_in_sources: Sequence[str]
    expected_build_scripts: Sequence[str]
    expected_owner_split: dict[str, list[str]]
    expected_machine_owned_output_roots: Sequence[str]
    expected_explicit_non_goals: Sequence[str]
    expected_checked_in_roots: Sequence[str]


def default_config() -> SourceSurfaceConfig:
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
