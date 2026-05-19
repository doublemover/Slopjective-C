from __future__ import annotations

from typing import Any

from objc3c_tooling.paths import resolve_repo_path

from capability_docs_validator.errors import CapabilityDocsError


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
