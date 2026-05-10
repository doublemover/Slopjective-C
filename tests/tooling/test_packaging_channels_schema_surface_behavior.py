from __future__ import annotations

from pathlib import Path

import pytest

from packaging_channels_schema_surface_behavior import (
    assert_packaging_channels_schema_surface_rejects_broken_registered_contract,
    assert_packaging_channels_schema_surface_rejects_broken_registered_draft,
    assert_packaging_channels_schema_surface_rejects_broken_registered_schema_id,
    assert_packaging_channels_schema_surface_rejects_metadata_required_field_drift,
    assert_packaging_channels_schema_surface_rejects_unregistered_surface_path,
    assert_packaging_channels_schema_surface_uses_registered_schemas,
)


def test_packaging_channels_schema_surface_uses_registered_schemas() -> None:
    assert_packaging_channels_schema_surface_uses_registered_schemas()


def test_packaging_channels_schema_surface_rejects_unregistered_surface_path(
    tmp_path: Path,
) -> None:
    assert_packaging_channels_schema_surface_rejects_unregistered_surface_path(tmp_path)


def test_packaging_channels_schema_surface_rejects_broken_registered_draft(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    assert_packaging_channels_schema_surface_rejects_broken_registered_draft(
        monkeypatch,
        tmp_path,
    )


def test_packaging_channels_schema_surface_rejects_broken_registered_schema_id(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    assert_packaging_channels_schema_surface_rejects_broken_registered_schema_id(
        monkeypatch,
        tmp_path,
    )


def test_packaging_channels_schema_surface_rejects_broken_registered_contract(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    assert_packaging_channels_schema_surface_rejects_broken_registered_contract(
        monkeypatch,
        tmp_path,
    )


def test_packaging_channels_schema_surface_rejects_metadata_required_field_drift(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    assert_packaging_channels_schema_surface_rejects_metadata_required_field_drift(
        monkeypatch,
        tmp_path,
    )
