"""Canonical checked-in schema path registry for tooling reports."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_any, load_json_object, validate_json_schema

ROOT = Path(__file__).resolve().parents[2]

SCHEMA_PATHS: dict[str, Path] = {
    "objc3c-governance-anti-regression-summary-v1": ROOT
    / "schemas"
    / "objc3c-governance-anti-regression-summary-v1.schema.json",
    "objc3c-governance-budget-summary-v1": ROOT
    / "schemas"
    / "objc3c-governance-budget-summary-v1.schema.json",
    "objc3c-governance-sustainability-evidence-v1": ROOT
    / "schemas"
    / "objc3c-governance-sustainability-evidence-v1.schema.json",
    "objc3c-public-command-contract-v1": ROOT / "schemas" / "objc3c-public-command-contract-v1.schema.json",
    "objc3c-public-conformance-summary-v1": ROOT
    / "schemas"
    / "objc3c-public-conformance-summary-v1.schema.json",
    "objc3c-runtime-performance-telemetry-v1": ROOT
    / "schemas"
    / "objc3c-runtime-performance-telemetry-v1.schema.json",
    "objc3c-validation-acceptance-artifact-index-v1": ROOT
    / "schemas"
    / "objc3c-validation-acceptance-artifact-index-v1.schema.json",
    "objc3c-capability-matrix-v1": ROOT / "docs" / "support" / "capability_matrix.schema.json",
}


def schema_path(schema_id: str) -> Path:
    try:
        return SCHEMA_PATHS[schema_id]
    except KeyError as exc:
        raise KeyError(f"unknown schema id: {schema_id}") from exc


def schema_ids() -> tuple[str, ...]:
    return tuple(sorted(SCHEMA_PATHS))


def load_schema(schema_id: str) -> dict[str, Any]:
    return load_json_object(schema_path(schema_id))


def validate_registered_schema(payload: Any, schema_id: str, *, label: str | None = None) -> None:
    validate_json_schema(payload, load_schema(schema_id), label=label or schema_id)


def load_json_with_registered_schema(path: Path | str, schema_id: str) -> Any:
    payload = load_json_any(path)
    validate_registered_schema(payload, schema_id, label=str(path))
    return payload


def schema_registry_summary() -> dict[str, str]:
    return {
        schema_id: path.relative_to(ROOT).as_posix()
        for schema_id, path in sorted(SCHEMA_PATHS.items())
    }
