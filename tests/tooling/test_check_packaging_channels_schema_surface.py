from __future__ import annotations

import importlib.util
import sys
from copy import deepcopy
from pathlib import Path
from typing import Any

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_shared.json_io import load_json_object
from objc3c_shared.json_io import write_json_file
from objc3c_shared.schema_registry import schema_path

SCRIPT_PATH = ROOT / "scripts" / "check_packaging_channels_schema_surface.py"


def _load_checker():
    spec = importlib.util.spec_from_file_location("check_packaging_channels_schema_surface", SCRIPT_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError("Unable to load scripts/check_packaging_channels_schema_surface.py")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def test_packaging_channels_schema_surface_uses_registered_schemas() -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = ROOT / "tmp" / "tests" / "packaging-channels-schema-surface-summary.json"
    checker.SUMMARY_PATH.unlink(missing_ok=True)

    try:
        assert checker.main() == 0

        summary = load_json_object(checker.SUMMARY_PATH)
        assert summary["contract_id"] == "objc3c.packaging.channels.schema.surface.summary.v1"
        assert summary["status"] == "PASS"
        assert summary["metadata_surface"] == "tests/tooling/fixtures/packaging_channels/metadata_surface.json"
        assert summary["schema_surface"] == "tests/tooling/fixtures/packaging_channels/schema_surface.json"
        assert summary["package_channels_manifest"] == (
            schema_path("objc3c-package-channels-manifest-v1").relative_to(ROOT).as_posix()
        )
        assert summary["install_receipt"] == (
            schema_path("objc3c-package-install-receipt-v1").relative_to(ROOT).as_posix()
        )
        assert summary["schema_count"] == 2
        assert summary["schemas"] == [
            schema_path("objc3c-package-channels-manifest-v1").relative_to(ROOT).as_posix(),
            schema_path("objc3c-package-install-receipt-v1").relative_to(ROOT).as_posix(),
        ]
        assert summary["schema_ids"] == [
            "https://objc3c.dev/schemas/objc3c-package-channels-manifest-v1.schema.json",
            "https://objc3c.dev/schemas/objc3c-package-install-receipt-v1.schema.json",
        ]
    finally:
        checker.SUMMARY_PATH.unlink(missing_ok=True)


def test_packaging_channels_schema_surface_rejects_unregistered_surface_path(
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    surface = load_json_object(checker.SCHEMA_SURFACE)
    surface["schemas"] = [
        "schemas/objc3c-package-channels-manifest-v1.schema.json",
        "schemas/unregistered-package-install-receipt.schema.json",
    ]

    checker.SCHEMA_SURFACE = tmp_path / "schema_surface.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json_file(checker.SCHEMA_SURFACE, surface, sort_keys=True)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_packaging_channels_schema_surface_rejects_broken_registered_draft(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    original_load_schema = checker.load_schema

    def broken_load_schema(schema_id: str) -> dict[str, Any]:
        payload = deepcopy(original_load_schema(schema_id))
        if schema_id == "objc3c-package-channels-manifest-v1":
            payload["$schema"] = "https://json-schema.org/draft/2019-09/schema"
        return payload

    monkeypatch.setattr(checker, "load_schema", broken_load_schema)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_packaging_channels_schema_surface_rejects_broken_registered_schema_id(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    original_load_schema = checker.load_schema

    def broken_load_schema(schema_id: str) -> dict[str, Any]:
        payload = deepcopy(original_load_schema(schema_id))
        if schema_id == "objc3c-package-install-receipt-v1":
            payload["$id"] = "https://objc3c.dev/schemas/objc3c-package-install-receipt-broken.schema.json"
        return payload

    monkeypatch.setattr(checker, "load_schema", broken_load_schema)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_packaging_channels_schema_surface_rejects_broken_registered_contract(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    original_load_schema = checker.load_schema

    def broken_load_schema(schema_id: str) -> dict[str, Any]:
        payload = deepcopy(original_load_schema(schema_id))
        if schema_id == "objc3c-package-channels-manifest-v1":
            payload["properties"]["contract_id"]["const"] = "objc3c.packaging.channels.broken.v1"
        return payload

    monkeypatch.setattr(checker, "load_schema", broken_load_schema)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_packaging_channels_schema_surface_rejects_metadata_required_field_drift(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    original_load_schema = checker.load_schema

    def broken_load_schema(schema_id: str) -> dict[str, Any]:
        payload = deepcopy(original_load_schema(schema_id))
        if schema_id == "objc3c-package-channels-manifest-v1":
            payload["required"].remove("platform_support_matrix")
        return payload

    monkeypatch.setattr(checker, "load_schema", broken_load_schema)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()
