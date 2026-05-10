from __future__ import annotations

from pathlib import Path

from external_validation_source_surface_behavior import (
    assert_external_validation_source_surface_owner_modules_are_importable,
    assert_external_validation_source_surface_rejects_checked_root_drift,
    assert_external_validation_source_surface_rejects_family_inventory_drift,
    assert_external_validation_source_surface_rejects_family_path_drift,
    assert_external_validation_source_surface_rejects_path_drift,
    assert_external_validation_source_surface_writes_named_summary_fields,
)


def test_external_validation_source_surface_owner_modules_are_importable() -> None:
    assert_external_validation_source_surface_owner_modules_are_importable()


def test_external_validation_source_surface_writes_named_summary_fields() -> None:
    assert_external_validation_source_surface_writes_named_summary_fields()


def test_external_validation_source_surface_rejects_path_drift(tmp_path: Path) -> None:
    assert_external_validation_source_surface_rejects_path_drift(tmp_path)


def test_external_validation_source_surface_rejects_checked_root_drift(
    tmp_path: Path,
) -> None:
    assert_external_validation_source_surface_rejects_checked_root_drift(tmp_path)


def test_external_validation_source_surface_rejects_family_inventory_drift(
    tmp_path: Path,
) -> None:
    assert_external_validation_source_surface_rejects_family_inventory_drift(tmp_path)


def test_external_validation_source_surface_rejects_family_path_drift(
    tmp_path: Path,
) -> None:
    assert_external_validation_source_surface_rejects_family_path_drift(tmp_path)
