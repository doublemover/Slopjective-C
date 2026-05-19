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

from objc3c_shared.json_io import load_json_object, write_json_file
from objc3c_shared.schema_registry import schema_path

SCRIPT_PATH = ROOT / "scripts" / "check_performance_governance_schema_surface.py"


def _load_checker():
    spec = importlib.util.spec_from_file_location("check_performance_governance_schema_surface", SCRIPT_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError("Unable to load scripts/check_performance_governance_schema_surface.py")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def test_performance_governance_schema_surface_uses_registered_schemas() -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = ROOT / "tmp" / "tests" / "performance-governance-schema-surface-summary.json"
    checker.SUMMARY_PATH.unlink(missing_ok=True)

    try:
        assert checker.main() == 0

        summary = load_json_object(checker.SUMMARY_PATH)
        assert summary["contract_id"] == "objc3c.performance.governance.schema.surface.summary.v1"
        assert summary["status"] == "PASS"
        assert summary["schema_surface"] == "tests/tooling/fixtures/performance_governance/schema_surface.json"
        assert summary["dashboard_summary_schema"] == (
            schema_path("objc3c-performance-dashboard-summary-v1").relative_to(ROOT).as_posix()
        )
        assert summary["public_report_schema"] == (
            schema_path("objc3c-performance-public-report-v1").relative_to(ROOT).as_posix()
        )
        assert summary["schemas"] == [
            schema_path("objc3c-performance-dashboard-summary-v1").relative_to(ROOT).as_posix(),
            schema_path("objc3c-performance-public-report-v1").relative_to(ROOT).as_posix(),
        ]
        assert summary["schema_ids"] == [
            "https://objc3c.dev/schemas/objc3c-performance-dashboard-summary-v1.schema.json",
            "https://objc3c.dev/schemas/objc3c-performance-public-report-v1.schema.json",
        ]
        surface = load_json_object(checker.SCHEMA_SURFACE)
        assert "schema_check_script" not in surface
        assert surface["schema_check_action"] == "check-performance-governance-schema-surface"
        assert surface["schema_check_command"] == (
            "npm run objc3c -- check-performance-governance-schema-surface"
        )
        assert surface["schema_check_implementation_anchor"] == (
            "scripts/check_performance_governance_schema_surface.py"
        )
    finally:
        checker.SUMMARY_PATH.unlink(missing_ok=True)


def test_performance_governance_schema_surface_rejects_retired_schema_check_script(
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    surface = load_json_object(checker.SCHEMA_SURFACE)
    surface["schema_check_script"] = "scripts/check_performance_governance_schema_surface.py"

    checker.SCHEMA_SURFACE = tmp_path / "schema_surface.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json_file(checker.SCHEMA_SURFACE, surface, sort_keys=True)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_performance_governance_schema_surface_rejects_action_command_anchor_drift(
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    surface = load_json_object(checker.SCHEMA_SURFACE)
    surface["schema_check_action"] = "scripts/check_performance_governance_schema_surface.py"

    checker.SCHEMA_SURFACE = tmp_path / "schema_surface.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json_file(checker.SCHEMA_SURFACE, surface, sort_keys=True)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_performance_governance_schema_surface_rejects_unregistered_surface_path(tmp_path: Path) -> None:
    checker = _load_checker()
    surface = load_json_object(checker.SCHEMA_SURFACE)
    surface["dashboard_summary_schema"] = "schemas/unregistered-performance-dashboard.schema.json"

    checker.SCHEMA_SURFACE = tmp_path / "schema_surface.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json_file(checker.SCHEMA_SURFACE, surface, sort_keys=True)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_performance_governance_schema_surface_rejects_broken_registered_draft(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    original_load_schema = checker.load_schema

    def broken_load_schema(schema_id: str) -> dict[str, Any]:
        payload = deepcopy(original_load_schema(schema_id))
        if schema_id == "objc3c-performance-dashboard-summary-v1":
            payload["$schema"] = "https://json-schema.org/draft/2019-09/schema"
        return payload

    monkeypatch.setattr(checker, "load_schema", broken_load_schema)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_performance_governance_schema_surface_rejects_broken_registered_schema_id(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    original_load_schema = checker.load_schema

    def broken_load_schema(schema_id: str) -> dict[str, Any]:
        payload = deepcopy(original_load_schema(schema_id))
        if schema_id == "objc3c-performance-public-report-v1":
            payload["$id"] = (
                "https://objc3c.dev/schemas/objc3c-performance-public-report-broken.schema.json"
            )
        return payload

    monkeypatch.setattr(checker, "load_schema", broken_load_schema)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_performance_governance_schema_surface_rejects_broken_registered_contract(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    checker = _load_checker()
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    original_load_schema = checker.load_schema

    def broken_load_schema(schema_id: str) -> dict[str, Any]:
        payload = deepcopy(original_load_schema(schema_id))
        if schema_id == "objc3c-performance-public-report-v1":
            payload["properties"]["contract_id"]["const"] = (
                "objc3c.performance.governance.public.broken.v1"
            )
        return payload

    monkeypatch.setattr(checker, "load_schema", broken_load_schema)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()
