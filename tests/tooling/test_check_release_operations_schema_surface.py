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
SCRIPT_PATH = ROOT / "scripts" / "check_release_operations_schema_surface.py"


def _load_checker():
    spec = importlib.util.spec_from_file_location("check_release_operations_schema_surface", SCRIPT_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError("Unable to load scripts/check_release_operations_schema_surface.py")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def test_release_operations_schema_surface_uses_registered_schemas() -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = ROOT / "tmp" / "tests" / "release-operations-schema-surface-summary.json"
    checker.SUMMARY_PATH.unlink(missing_ok=True)

    try:
        assert checker.main() == 0

        summary = load_json_object(checker.SUMMARY_PATH)
        assert summary["contract_id"] == "objc3c.release.operations.schema.surface.summary.v1"
        assert summary["status"] == "PASS"
        assert summary["metadata_surface"] == "tests/tooling/fixtures/release_operations/metadata_surface.json"
        assert summary["schema_surface"] == "tests/tooling/fixtures/release_operations/schema_surface.json"
        assert summary["update_manifest"] == "schemas/objc3c-update-manifest-v1.schema.json"
        assert summary["upgrade_support_report"] == (
            "schemas/objc3c-upgrade-support-report-v1.schema.json"
        )
        assert summary["schemas"] == [
            "schemas/objc3c-update-manifest-v1.schema.json",
            "schemas/objc3c-upgrade-support-report-v1.schema.json",
        ]
        assert summary["schema_ids"] == [
            "https://objc3c.dev/schemas/objc3c-update-manifest-v1.schema.json",
            "https://objc3c.dev/schemas/objc3c-upgrade-support-report-v1.schema.json",
        ]
    finally:
        checker.SUMMARY_PATH.unlink(missing_ok=True)


def test_release_operations_schema_surface_rejects_unregistered_surface_path(
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    surface = load_json_object(checker.SCHEMA_SURFACE)
    surface["upgrade_support_report"] = "schemas/unregistered-upgrade-support-report.schema.json"

    checker.SCHEMA_SURFACE = tmp_path / "schema_surface.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json_file(checker.SCHEMA_SURFACE, surface, sort_keys=True)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_release_operations_schema_surface_rejects_broken_registered_draft(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    original_load_json = checker.load_json

    def broken_load_json(path: Path) -> dict[str, Any]:
        payload = deepcopy(original_load_json(path))
        if path.as_posix().endswith("schemas/objc3c-update-manifest-v1.schema.json"):
            payload["$schema"] = "https://json-schema.org/draft/2019-09/schema"
        return payload

    monkeypatch.setattr(checker, "load_json", broken_load_json)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_release_operations_schema_surface_rejects_broken_registered_schema_id(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    original_load_json = checker.load_json

    def broken_load_json(path: Path) -> dict[str, Any]:
        payload = deepcopy(original_load_json(path))
        if path.as_posix().endswith("schemas/objc3c-upgrade-support-report-v1.schema.json"):
            payload["$id"] = "https://objc3c.dev/schemas/objc3c-upgrade-support-report-broken.schema.json"
        return payload

    monkeypatch.setattr(checker, "load_json", broken_load_json)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_release_operations_schema_surface_rejects_broken_registered_contract(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    original_load_json = checker.load_json

    def broken_load_json(path: Path) -> dict[str, Any]:
        payload = deepcopy(original_load_json(path))
        if path.as_posix().endswith("schemas/objc3c-update-manifest-v1.schema.json"):
            payload["properties"]["contract_id"]["const"] = (
                "objc3c.release.operations.update-manifest.broken.v1"
            )
        return payload

    monkeypatch.setattr(checker, "load_json", broken_load_json)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_release_operations_schema_surface_rejects_metadata_required_field_drift(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    original_load_json = checker.load_json

    def broken_load_json(path: Path) -> dict[str, Any]:
        payload = deepcopy(original_load_json(path))
        if path.as_posix().endswith("schemas/objc3c-upgrade-support-report-v1.schema.json"):
            payload["required"].remove("platform_support_matrix")
        return payload

    monkeypatch.setattr(checker, "load_json", broken_load_json)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()
