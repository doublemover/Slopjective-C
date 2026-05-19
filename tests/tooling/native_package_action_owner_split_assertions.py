from __future__ import annotations

from typing import Iterable


def assert_contains_all(text: str, snippets: Iterable[str]) -> None:
    for snippet in snippets:
        assert snippet in text


def assert_excludes_all(text: str, snippets: Iterable[str]) -> None:
    for snippet in snippets:
        assert snippet not in text


def assert_workflow_modules_import(module_names: Iterable[str]) -> None:
    from native_package_action_owner_split_sources import import_workflow_owner_module

    for module_name in module_names:
        assert import_workflow_owner_module(module_name)
