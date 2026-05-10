from __future__ import annotations

from pathlib import Path

from application_materialization_owner_split_assertions import (
    assert_project_template_manifest_payload,
    assert_script_delegates_to_owner_symbols,
    assert_stdlib_workspace_summary_payload,
)
from application_materialization_owner_split_sources import (
    entrypoint_owner_imports,
    entrypoint_script_text,
    import_owner_module,
    owner_module_names,
    project_template_manifest_for_tmp,
    stdlib_workspace_summary_for_tmp,
)


def assert_materialization_owner_modules_are_explicit() -> None:
    for module_name in owner_module_names():
        assert import_owner_module(module_name)


def assert_materialization_entrypoints_delegate_to_owner_modules() -> None:
    for relative_path, owner_symbols in entrypoint_owner_imports().items():
        assert_script_delegates_to_owner_symbols(
            entrypoint_script_text(relative_path),
            owner_symbols,
        )


def assert_project_template_manifest_is_owned_by_manifest_builder(
    tmp_path: Path,
) -> None:
    assert_project_template_manifest_payload(project_template_manifest_for_tmp(tmp_path))


def assert_stdlib_summary_contract_dedupes_materialized_paths(
    tmp_path: Path,
) -> None:
    assert_stdlib_workspace_summary_payload(stdlib_workspace_summary_for_tmp(tmp_path))
