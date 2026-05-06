"""Canonical checked-in schema path registry for tooling reports."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.paths import ROOT

SCHEMA_PATHS: dict[str, Path] = {
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


def schema_registry_summary() -> dict[str, str]:
    return {schema_id: path.relative_to(ROOT).as_posix() for schema_id, path in sorted(SCHEMA_PATHS.items())}
