from __future__ import annotations

from pathlib import Path
from types import ModuleType
from typing import Any

from external_validation_source_surface_assertions import (
    assert_fail_closed_without_summary,
    assert_named_summary_fields,
    assert_owner_modules_match_checker,
)
from external_validation_source_surface_sources import (
    load_checker,
    load_summary,
    load_surface,
    temporary_summary_path,
    write_surface,
)


def assert_external_validation_source_surface_owner_modules_are_importable() -> None:
    assert_owner_modules_match_checker(load_checker())


def assert_external_validation_source_surface_writes_named_summary_fields() -> None:
    checker = load_checker()
    checker.SUMMARY_PATH = temporary_summary_path()
    checker.SUMMARY_PATH.unlink(missing_ok=True)

    try:
        assert checker.main() == 0
        assert_named_summary_fields(load_summary(checker.SUMMARY_PATH), checker)
    finally:
        checker.SUMMARY_PATH.unlink(missing_ok=True)


def assert_external_validation_source_surface_rejects_path_drift(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    surface = load_surface(checker)
    surface["trust_policy"] = (
        "tests/tooling/fixtures/external_validation/legacy_trust_policy.json"
    )
    write_mutated_surface(checker, tmp_path, surface)
    assert_fail_closed_without_summary(checker)


def assert_external_validation_source_surface_rejects_checked_root_drift(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    surface = load_surface(checker)
    surface["checked_in_roots"] = [
        *checker.EXPECTED_ROOTS,
        "tests/tooling/fixtures/external_validation_compat",
    ]
    write_mutated_surface(checker, tmp_path, surface)
    assert_fail_closed_without_summary(checker)


def assert_external_validation_source_surface_rejects_family_inventory_drift(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    surface = load_surface(checker)
    source_families = list(surface["source_families"])
    surface["source_families"] = [
        source_families[1],
        source_families[0],
        source_families[2],
    ]
    write_mutated_surface(checker, tmp_path, surface)
    assert_fail_closed_without_summary(checker)


def assert_external_validation_source_surface_rejects_family_path_drift(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    surface = load_surface(checker)
    source_families = []
    for family in surface["source_families"]:
        copied_family = dict(family)
        copied_family["source_paths"] = list(family["source_paths"])
        source_families.append(copied_family)
    source_families[0]["source_paths"].append(
        "tests/tooling/fixtures/external_validation/legacy_surface.json"
    )
    surface["source_families"] = source_families
    write_mutated_surface(checker, tmp_path, surface)
    assert_fail_closed_without_summary(checker)


def write_mutated_surface(
    checker: ModuleType,
    tmp_path: Path,
    surface: dict[str, Any],
) -> None:
    checker.SOURCE_SURFACE = tmp_path / "source_surface.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_surface(checker.SOURCE_SURFACE, surface)
