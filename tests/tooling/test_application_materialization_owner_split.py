from __future__ import annotations

from pathlib import Path

from application_materialization_owner_split_behavior import (
    assert_materialization_entrypoints_delegate_to_owner_modules,
    assert_materialization_owner_modules_are_explicit,
    assert_project_template_manifest_is_owned_by_manifest_builder,
    assert_stdlib_summary_contract_dedupes_materialized_paths,
)


def test_materialization_owner_modules_are_explicit() -> None:
    assert_materialization_owner_modules_are_explicit()


def test_materialization_entrypoints_delegate_to_owner_modules() -> None:
    assert_materialization_entrypoints_delegate_to_owner_modules()


def test_project_template_manifest_is_owned_by_manifest_builder(tmp_path: Path) -> None:
    assert_project_template_manifest_is_owned_by_manifest_builder(tmp_path)


def test_stdlib_summary_contract_dedupes_materialized_paths(tmp_path: Path) -> None:
    assert_stdlib_summary_contract_dedupes_materialized_paths(tmp_path)
