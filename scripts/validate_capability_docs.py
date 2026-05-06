#!/usr/bin/env python3
"""Validate canonical capability docs and evidence links."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Any, Sequence

from objc3c_shared.json_io import (
    JsonSchemaValidationError,
    load_json_object,
    validate_json_schema,
)
from objc3c_tooling.paths import ROOT, display_path, resolve_repo_path

MATRIX_PATH = ROOT / "docs" / "support" / "capability_matrix.json"
SCHEMA_PATH = ROOT / "docs" / "support" / "capability_matrix.schema.json"
MATRIX_DOC = ROOT / "docs" / "support" / "capability_matrix.md"
EVIDENCE_DOC = ROOT / "docs" / "support" / "evidence_map.md"


class CapabilityDocsError(RuntimeError):
    pass


def _require_matrix_shape(matrix: dict[str, Any]) -> list[dict[str, Any]]:
    capabilities = matrix.get("capabilities")
    if not isinstance(capabilities, list):
        raise CapabilityDocsError("capabilities must be a list")
    rows: list[dict[str, Any]] = []
    seen: set[str] = set()
    for index, row in enumerate(capabilities):
        if not isinstance(row, dict):
            raise CapabilityDocsError(f"capabilities[{index}] must be an object")
        capability_id = row.get("id")
        if not isinstance(capability_id, str):
            raise CapabilityDocsError(f"capabilities[{index}].id must be a string")
        if capability_id in seen:
            raise CapabilityDocsError(f"duplicate capability id: {capability_id}")
        seen.add(capability_id)
        rows.append(row)
    return rows


def _validate_evidence_rows(rows: list[dict[str, Any]]) -> None:
    for row in rows:
        capability_id = str(row["id"])
        state = str(row["state"])
        evidence = row["evidence"]
        if not isinstance(evidence, list) or not evidence:
            raise CapabilityDocsError(f"{capability_id} must have evidence")
        kinds = {str(item.get("kind")) for item in evidence if isinstance(item, dict)}
        if state == "implemented" and "test" not in kinds:
            raise CapabilityDocsError(f"{capability_id} implemented rows require test evidence")
        if state == "reserved" and kinds.isdisjoint({"diagnostic", "doc"}):
            raise CapabilityDocsError(f"{capability_id} reserved rows require diagnostic or doc evidence")
        for item in evidence:
            if not isinstance(item, dict):
                raise CapabilityDocsError(f"{capability_id} evidence entries must be objects")
            raw_path = item.get("path")
            if not isinstance(raw_path, str) or not raw_path:
                raise CapabilityDocsError(f"{capability_id} evidence path must be non-empty")
            path = resolve_repo_path(raw_path)
            if not path.exists():
                raise CapabilityDocsError(f"{capability_id} evidence path is missing: {raw_path}")


def _validate_docs_reference_rows(rows: list[dict[str, Any]]) -> None:
    matrix_text = MATRIX_DOC.read_text(encoding="utf-8")
    evidence_text = EVIDENCE_DOC.read_text(encoding="utf-8")
    for row in rows:
        capability_id = str(row["id"])
        if capability_id not in evidence_text:
            raise CapabilityDocsError(f"evidence map missing capability id: {capability_id}")
        for item in row["evidence"]:
            path = str(item["path"])
            if path not in evidence_text and path not in matrix_text:
                raise CapabilityDocsError(f"docs missing evidence path for {capability_id}: {path}")


def validate() -> None:
    matrix = load_json_object(MATRIX_PATH)
    schema = load_json_object(SCHEMA_PATH)
    try:
        validate_json_schema(matrix, schema, label=display_path(MATRIX_PATH))
    except JsonSchemaValidationError as exc:
        raise CapabilityDocsError(str(exc)) from exc
    rows = _require_matrix_shape(matrix)
    _validate_evidence_rows(rows)
    _validate_docs_reference_rows(rows)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true", help="validate capability docs")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    _ = build_parser().parse_args(argv)
    try:
        validate()
    except CapabilityDocsError as exc:
        print(f"capability docs validation error: {exc}", file=sys.stderr)
        return 1
    print("capability-docs: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
