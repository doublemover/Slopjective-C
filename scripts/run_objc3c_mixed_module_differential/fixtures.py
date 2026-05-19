"""Manifest validation and run-source preparation for mixed-module differential runs."""

from __future__ import annotations

from datetime import datetime
from pathlib import Path
from typing import Any

from .commands import CASE_RUNNERS, SURFACE_BUILDERS
from .config import ROOT
from .models import FixtureGroup
from .tooling import load_json

MANIFEST_CONTRACT_ID = "objc3c.stress.mixed-module-differential.manifest.v1"
MANIFEST_SCHEMA_VERSION = 1


def _require_object(payload: Any, label: str) -> dict[str, Any]:
    if not isinstance(payload, dict):
        raise RuntimeError(f"mixed-module differential {label} was not an object")
    return payload


def _load_case_ids(payload: dict[str, Any]) -> list[str]:
    case_ids = payload.get("case_ids")
    if not isinstance(case_ids, list) or not case_ids:
        raise RuntimeError("mixed-module differential manifest missing case_ids")
    normalized: list[str] = []
    for case_id in case_ids:
        if not isinstance(case_id, str) or case_id not in CASE_RUNNERS:
            raise RuntimeError(f"mixed-module differential manifest references unsupported case {case_id!r}")
        normalized.append(case_id)
    return normalized


def _load_surface_contracts(payload: dict[str, Any]) -> dict[str, str]:
    surface_contracts = payload.get("surface_contracts")
    if not isinstance(surface_contracts, dict) or not surface_contracts:
        raise RuntimeError("mixed-module differential manifest missing surface_contracts")

    normalized: dict[str, str] = {}
    for surface_key, contract_id in surface_contracts.items():
        if not isinstance(surface_key, str) or surface_key not in SURFACE_BUILDERS:
            raise RuntimeError(f"mixed-module differential manifest references unsupported surface {surface_key!r}")
        if not isinstance(contract_id, str) or not contract_id:
            raise RuntimeError(f"mixed-module differential manifest surface {surface_key!r} has invalid contract")
        normalized[surface_key] = contract_id
    return normalized


def _load_fixture_groups(payload: dict[str, Any]) -> list[dict[str, str]]:
    fixture_groups = payload.get("fixture_groups")
    if not isinstance(fixture_groups, list) or not fixture_groups:
        raise RuntimeError("mixed-module differential manifest missing fixture_groups")
    return [FixtureGroup.from_manifest(ROOT, group).to_json() for group in fixture_groups]


def load_manifest(path: Path) -> dict[str, Any]:
    payload = _require_object(load_json(path), "manifest")
    if payload.get("contract_id") != MANIFEST_CONTRACT_ID:
        raise RuntimeError("mixed-module differential manifest contract_id drifted")
    if payload.get("schema_version") != MANIFEST_SCHEMA_VERSION:
        raise RuntimeError("mixed-module differential manifest schema_version drifted")
    return {
        "case_ids": _load_case_ids(payload),
        "surface_contracts": _load_surface_contracts(payload),
        "fixture_groups": _load_fixture_groups(payload),
    }


def prepare_run_root(run_id: str | None = None) -> Path:
    stable_run_id = run_id or datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    run_root = ROOT / "tmp" / "artifacts" / "stress" / "mixed-module-differential" / stable_run_id
    run_root.mkdir(parents=True, exist_ok=True)
    return run_root


__all__ = ["MANIFEST_CONTRACT_ID", "MANIFEST_SCHEMA_VERSION", "load_manifest", "prepare_run_root"]
