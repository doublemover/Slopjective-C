"""Manifest and fixture-surface loading for stress minimization."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json

from .models import MinCase
from .paths import ROOT

ALLOWED_MINIMIZATION_SUBSYSTEMS = {"parser", "semantic", "runtime", "execution"}


def load_manifest(path: Path) -> list[MinCase]:
    payload = load_json(path)
    if payload.get("contract_id") != "objc3c.stress.minimization.manifest.v1":
        raise RuntimeError("stress minimization manifest contract_id drifted")
    if payload.get("schema_version") != 1:
        raise RuntimeError("stress minimization manifest schema_version drifted")
    cases = payload.get("cases")
    if not isinstance(cases, list) or not cases:
        raise RuntimeError("stress minimization manifest missing cases")
    selected: list[MinCase] = []
    for item in cases:
        if not isinstance(item, dict):
            raise RuntimeError("stress minimization manifest contains a non-object case")
        case_id = item.get("case_id")
        subsystem = item.get("subsystem")
        source_path = item.get("source_path")
        if not isinstance(case_id, str) or not case_id:
            raise RuntimeError("stress minimization manifest case missing case_id")
        if subsystem not in ALLOWED_MINIMIZATION_SUBSYSTEMS:
            raise RuntimeError(f"stress minimization manifest case {case_id} has invalid subsystem")
        if not isinstance(source_path, str) or not source_path:
            raise RuntimeError(f"stress minimization manifest case {case_id} missing source_path")
        resolved = (ROOT / source_path).resolve()
        if not resolved.is_file():
            raise RuntimeError(f"stress minimization manifest references missing source {source_path}")
        selected.append(MinCase(case_id=case_id, subsystem=subsystem, source_path=resolved))
    return selected


def validate_artifact_surface(path: Path) -> dict[str, Any]:
    payload = load_json(path)
    if payload.get("contract_id") != "objc3c.stress.artifact.surface.v1":
        raise RuntimeError("stress artifact surface contract_id drifted")
    if payload.get("schema_version") != 1:
        raise RuntimeError("stress artifact surface schema_version drifted")
    return payload


__all__ = ["load_manifest", "validate_artifact_surface"]
