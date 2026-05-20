"""Validation runner for the performance-governance source surface."""

from __future__ import annotations

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_tooling.paths import repo_rel

from .config import SourceSurfaceConfig
from .summary import build_summary_payload
from .validation import (
    FailureHandler,
    fail,
    require_exact_list,
    require_exact_owner_split,
    require_exact_path,
    require_path,
)


def _display_path(path: object) -> str:
    try:
        return repo_rel(path)
    except ValueError:
        return str(path)


def run(
    config: SourceSurfaceConfig,
    *,
    fail_handler: FailureHandler = fail,
) -> int:
    if not config.source_surface.is_file():
        return fail_handler(
            f"missing source surface contract: {repo_rel(config.source_surface)}"
        )

    surface = load_json(config.source_surface)
    if surface.get("contract_id") != config.source_surface_contract_id:
        return fail_handler("contract_id drifted")
    if surface.get("surface_kind") != config.source_surface_kind:
        return fail_handler("surface_kind drifted")

    checked_paths = [_display_path(config.source_surface)]
    for field_name in config.expected_required_paths:
        relative_path = require_exact_path(
            surface,
            field_name,
            expected_required_paths=config.expected_required_paths,
            fail_handler=fail_handler,
        )
        if relative_path is None:
            return 1
        if not require_path(
            relative_path,
            kind=field_name,
            root=config.root,
            fail_handler=fail_handler,
        ):
            return 1
        checked_paths.append(relative_path)

    checked_in_sources = require_exact_list(
        surface,
        "checked_in_sources",
        config.expected_checked_in_sources,
        fail_handler=fail_handler,
    )
    build_scripts = require_exact_list(
        surface,
        "build_scripts",
        config.expected_build_scripts,
        fail_handler=fail_handler,
    )
    upstream_reports = require_exact_list(
        surface,
        "upstream_reports",
        config.expected_upstream_reports,
        fail_handler=fail_handler,
    )
    machine_owned_output_roots = require_exact_list(
        surface,
        "machine_owned_output_roots",
        config.expected_machine_owned_output_roots,
        fail_handler=fail_handler,
    )
    owner_split = require_exact_owner_split(
        surface,
        expected_owner_split=config.expected_owner_split,
        fail_handler=fail_handler,
    )
    explicit_non_goals = require_exact_list(
        surface,
        "explicit_non_goals",
        config.expected_explicit_non_goals,
        fail_handler=fail_handler,
    )
    if (
        checked_in_sources is None
        or build_scripts is None
        or upstream_reports is None
        or machine_owned_output_roots is None
        or owner_split is None
        or explicit_non_goals is None
    ):
        return 1

    for list_name, items in (
        ("checked_in_sources", checked_in_sources),
        ("build_scripts", build_scripts),
        ("checked_in_roots", config.expected_checked_in_roots),
    ):
        for relative_path in items:
            if not require_path(
                relative_path,
                kind=list_name,
                root=config.root,
                fail_handler=fail_handler,
            ):
                return 1
            checked_paths.append(relative_path)

    for owner_name, owner_paths in owner_split.items():
        for relative_path in owner_paths:
            if not require_path(
                relative_path,
                kind=f"{owner_name} owner_split",
                root=config.root,
                fail_handler=fail_handler,
            ):
                return 1
            checked_paths.append(relative_path)

    summary = build_summary_payload(
        summary_contract_id=config.summary_contract_id,
        source_surface=config.source_surface,
        expected_runbook=config.expected_runbook,
        expected_required_paths=config.expected_required_paths,
        checked_in_sources=checked_in_sources,
        expected_checked_in_roots=config.expected_checked_in_roots,
        owner_split=owner_split,
        build_scripts=build_scripts,
        upstream_reports=upstream_reports,
        machine_owned_output_roots=machine_owned_output_roots,
        explicit_non_goals=explicit_non_goals,
        checked_paths=checked_paths,
    )
    write_report_json(config.summary_path, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(config.summary_path)}")
    print("performance-governance-source-surface: OK")
    return 0
