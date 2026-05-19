from __future__ import annotations

from copy import deepcopy
from pathlib import Path
from types import ModuleType
from typing import Any, Callable

import pytest

from release_operations_schema_surface_assertions import (
    assert_fail_closed_without_summary,
    assert_summary_matches_registered_schemas,
)
from release_operations_schema_surface_sources import (
    load_checker,
    load_json,
    temporary_summary_path,
    write_json,
)


def assert_release_operations_schema_surface_uses_registered_schemas() -> None:
    checker = load_checker()
    checker.SUMMARY_PATH = temporary_summary_path()
    checker.SUMMARY_PATH.unlink(missing_ok=True)

    try:
        assert checker.main() == 0
        assert_summary_matches_registered_schemas(load_json(checker.SUMMARY_PATH))
    finally:
        checker.SUMMARY_PATH.unlink(missing_ok=True)


def assert_release_operations_schema_surface_rejects_unregistered_surface_path(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    surface = load_json(checker.SCHEMA_SURFACE)
    surface["upgrade_support_report"] = (
        "schemas/unregistered-upgrade-support-report.schema.json"
    )

    checker.SCHEMA_SURFACE = tmp_path / "schema_surface.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json(checker.SCHEMA_SURFACE, surface)

    assert_fail_closed_without_summary(checker)


def assert_release_operations_schema_surface_rejects_broken_registered_draft(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    checker = checker_with_broken_schema(
        monkeypatch,
        tmp_path,
        "schemas/objc3c-update-manifest-v1.schema.json",
        lambda payload: payload.__setitem__(
            "$schema",
            "https://json-schema.org/draft/2019-09/schema",
        ),
    )

    assert_fail_closed_without_summary(checker)


def assert_release_operations_schema_surface_rejects_broken_registered_schema_id(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    checker = checker_with_broken_schema(
        monkeypatch,
        tmp_path,
        "schemas/objc3c-upgrade-support-report-v1.schema.json",
        lambda payload: payload.__setitem__(
            "$id",
            "https://objc3c.dev/schemas/objc3c-upgrade-support-report-broken.schema.json",
        ),
    )

    assert_fail_closed_without_summary(checker)


def assert_release_operations_schema_surface_rejects_broken_registered_contract(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    def break_contract(payload: dict[str, Any]) -> None:
        payload["properties"]["contract_id"]["const"] = (
            "objc3c.release.operations.update-manifest.broken.v1"
        )

    checker = checker_with_broken_schema(
        monkeypatch,
        tmp_path,
        "schemas/objc3c-update-manifest-v1.schema.json",
        break_contract,
    )

    assert_fail_closed_without_summary(checker)


def assert_release_operations_schema_surface_rejects_metadata_required_field_drift(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    def remove_required_field(payload: dict[str, Any]) -> None:
        payload["required"].remove("platform_support_matrix")

    checker = checker_with_broken_schema(
        monkeypatch,
        tmp_path,
        "schemas/objc3c-upgrade-support-report-v1.schema.json",
        remove_required_field,
    )

    assert_fail_closed_without_summary(checker)


def checker_with_broken_schema(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
    schema_path_suffix: str,
    mutate_payload: Callable[[dict[str, Any]], None],
) -> ModuleType:
    checker = load_checker()
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    original_load_json = checker.load_json

    def broken_load_json(path: Path) -> dict[str, Any]:
        payload = deepcopy(original_load_json(path))
        if path.as_posix().endswith(schema_path_suffix):
            mutate_payload(payload)
        return payload

    monkeypatch.setattr(checker, "load_json", broken_load_json)
    return checker
