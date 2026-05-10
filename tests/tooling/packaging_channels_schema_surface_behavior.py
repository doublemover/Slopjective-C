from __future__ import annotations

from copy import deepcopy
from pathlib import Path
from types import ModuleType
from typing import Any, Callable

import pytest

from packaging_channels_schema_surface_assertions import (
    assert_fail_closed_without_summary,
    assert_summary_matches_registered_schemas,
)
from packaging_channels_schema_surface_sources import (
    load_checker,
    load_json,
    temporary_summary_path,
    write_json,
)


def assert_packaging_channels_schema_surface_uses_registered_schemas() -> None:
    checker = load_checker()
    checker.SUMMARY_PATH = temporary_summary_path()
    checker.SUMMARY_PATH.unlink(missing_ok=True)

    try:
        assert checker.main() == 0
        assert_summary_matches_registered_schemas(load_json(checker.SUMMARY_PATH))
    finally:
        checker.SUMMARY_PATH.unlink(missing_ok=True)


def assert_packaging_channels_schema_surface_rejects_unregistered_surface_path(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    surface = load_json(checker.SCHEMA_SURFACE)
    surface["schemas"] = [
        "schemas/objc3c-package-channels-manifest-v1.schema.json",
        "schemas/unregistered-package-install-receipt.schema.json",
    ]

    checker.SCHEMA_SURFACE = tmp_path / "schema_surface.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json(checker.SCHEMA_SURFACE, surface)

    assert_fail_closed_without_summary(checker)


def assert_packaging_channels_schema_surface_rejects_broken_registered_draft(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    checker = checker_with_broken_schema(
        monkeypatch,
        tmp_path,
        "objc3c-package-channels-manifest-v1",
        lambda payload: payload.__setitem__(
            "$schema",
            "https://json-schema.org/draft/2019-09/schema",
        ),
    )

    assert_fail_closed_without_summary(checker)


def assert_packaging_channels_schema_surface_rejects_broken_registered_schema_id(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    checker = checker_with_broken_schema(
        monkeypatch,
        tmp_path,
        "objc3c-package-install-receipt-v1",
        lambda payload: payload.__setitem__(
            "$id",
            "https://objc3c.dev/schemas/objc3c-package-install-receipt-broken.schema.json",
        ),
    )

    assert_fail_closed_without_summary(checker)


def assert_packaging_channels_schema_surface_rejects_broken_registered_contract(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    def break_contract(payload: dict[str, Any]) -> None:
        payload["properties"]["contract_id"]["const"] = (
            "objc3c.packaging.channels.broken.v1"
        )

    checker = checker_with_broken_schema(
        monkeypatch,
        tmp_path,
        "objc3c-package-channels-manifest-v1",
        break_contract,
    )

    assert_fail_closed_without_summary(checker)


def assert_packaging_channels_schema_surface_rejects_metadata_required_field_drift(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    def remove_required_field(payload: dict[str, Any]) -> None:
        payload["required"].remove("platform_support_matrix")

    checker = checker_with_broken_schema(
        monkeypatch,
        tmp_path,
        "objc3c-package-channels-manifest-v1",
        remove_required_field,
    )

    assert_fail_closed_without_summary(checker)


def checker_with_broken_schema(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
    broken_schema_id: str,
    mutate_payload: Callable[[dict[str, Any]], None],
) -> ModuleType:
    checker = load_checker()
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    original_load_schema = checker.load_schema

    def broken_load_schema(schema_id: str) -> dict[str, Any]:
        payload = deepcopy(original_load_schema(schema_id))
        if schema_id == broken_schema_id:
            mutate_payload(payload)
        return payload

    monkeypatch.setattr(checker, "load_schema", broken_load_schema)
    return checker
