from __future__ import annotations

from copy import deepcopy
from pathlib import Path
from types import ModuleType
from typing import Any, Callable

import pytest

from release_foundation_schema_surface_assertions import (
    assert_fail_closed_without_summary,
    assert_summary_matches_registered_schemas,
)
from release_foundation_schema_surface_sources import (
    load_checker,
    load_json,
    temporary_summary_path,
    write_json,
)


def assert_release_foundation_schema_surface_uses_registered_schemas() -> None:
    checker = load_checker()
    checker.SUMMARY_PATH = temporary_summary_path()
    checker.SUMMARY_PATH.unlink(missing_ok=True)

    try:
        assert checker.main() == 0
        assert_summary_matches_registered_schemas(load_json(checker.SUMMARY_PATH))
    finally:
        checker.SUMMARY_PATH.unlink(missing_ok=True)


def assert_release_foundation_schema_surface_rejects_unregistered_surface_path(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    surface = load_json(checker.SCHEMA_SURFACE)
    surface["release_sbom_schema"] = "schemas/unregistered-release-sbom.schema.json"

    checker.SCHEMA_SURFACE = tmp_path / "schema_surface.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json(checker.SCHEMA_SURFACE, surface)

    assert_fail_closed_without_summary(checker)


def assert_release_foundation_schema_surface_rejects_broken_registered_draft(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    checker = checker_with_broken_schema(
        monkeypatch,
        tmp_path,
        "objc3c-release-manifest-v1",
        lambda payload: payload.__setitem__(
            "$schema",
            "https://json-schema.org/draft/2019-09/schema",
        ),
    )

    assert_fail_closed_without_summary(checker)


def assert_release_foundation_schema_surface_rejects_broken_registered_schema_id(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    checker = checker_with_broken_schema(
        monkeypatch,
        tmp_path,
        "objc3c-release-sbom-v1",
        lambda payload: payload.__setitem__(
            "$id",
            "https://objc3c.dev/schemas/objc3c-release-sbom-broken.schema.json",
        ),
    )

    assert_fail_closed_without_summary(checker)


def assert_release_foundation_schema_surface_rejects_broken_registered_contract(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    def break_contract(payload: dict[str, Any]) -> None:
        payload["properties"]["contract_id"]["const"] = (
            "objc3c.release.foundation.attestation.broken.v1"
        )

    checker = checker_with_broken_schema(
        monkeypatch,
        tmp_path,
        "objc3c-release-attestation-v1",
        break_contract,
    )

    assert_fail_closed_without_summary(checker)


def assert_release_foundation_schema_surface_rejects_abi_manifest_identity_drift(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    def break_abi_identity(payload: dict[str, Any]) -> None:
        payload["properties"]["manifest_schema"]["const"] = "objc3-abi-broken"

    checker = checker_with_broken_schema(
        monkeypatch,
        tmp_path,
        "objc3-abi-2025Q4",
        break_abi_identity,
    )

    assert_fail_closed_without_summary(checker)


def assert_release_foundation_schema_surface_rejects_runtime_abi_manifest_drift(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    def break_runtime_identity(payload: dict[str, Any]) -> None:
        payload["$defs"]["runtimeManifest"]["properties"]["artifact_id"]["const"] = (
            "objc3-runtime-broken"
        )

    checker = checker_with_broken_schema(
        monkeypatch,
        tmp_path,
        "objc3-runtime-2025Q4-manifest",
        break_runtime_identity,
    )

    assert_fail_closed_without_summary(checker)


def assert_release_foundation_schema_surface_rejects_package_attestation_drift(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    def remove_release_manifest_binding(payload: dict[str, Any]) -> None:
        artifacts = payload["properties"]["channels"]["items"]["properties"]["artifacts"]
        artifacts["required"].remove("release_manifest")

    checker = checker_with_broken_schema(
        monkeypatch,
        tmp_path,
        "objc3c-update-manifest-v1",
        remove_release_manifest_binding,
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
