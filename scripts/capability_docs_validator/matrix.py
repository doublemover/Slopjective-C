from __future__ import annotations

from typing import Any

from objc3c_tooling.paths import resolve_repo_path

from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.support_links import _row_support_claims

OBJECT_MODEL_IMPLEMENTED_PREFIX = "runtime.object-model."
OBJECT_MODEL_SUPPORT_CLAIM_PREFIX = "objc3c.behavior.runtime.object-model-"
OBJECT_MODEL_RUNTIME_FIXTURE_PREFIX = "tests/native/runtime/object_model/"
OBJECT_MODEL_RUNTIME_SOURCE_PREFIX = "native/objc3c/src/runtime/classes/"
OBJECT_MODEL_FULL_REALIZATION_ID = "runtime.object-model.full-realization"
OBJECT_MODEL_BROAD_SCOPE_PHRASES = (
    "full object-model",
    "full runtime object",
    "full live class",
    "object-model runtime realization",
    "broad object-model",
    "public reflection abi",
)


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


def _validate_object_model_scope(rows: list[dict[str, Any]]) -> None:
    for row in rows:
        capability_id = str(row["id"])
        state = str(row["state"])
        if (
            state != "implemented"
            or not capability_id.startswith(OBJECT_MODEL_IMPLEMENTED_PREFIX)
            or capability_id == OBJECT_MODEL_FULL_REALIZATION_ID
        ):
            continue

        claims = _row_support_claims(row)
        if not claims or any(
            not claim.startswith(OBJECT_MODEL_SUPPORT_CLAIM_PREFIX)
            for claim in claims
        ):
            raise CapabilityDocsError(
                f"{capability_id} object-model support claims must use "
                f"{OBJECT_MODEL_SUPPORT_CLAIM_PREFIX}*"
            )

        title = str(row.get("title", ""))
        summary = str(row.get("summary", ""))
        scope_text = f"{title} {summary}".lower()
        for phrase in OBJECT_MODEL_BROAD_SCOPE_PHRASES:
            if phrase in scope_text:
                raise CapabilityDocsError(
                    f"{capability_id} object-model implemented rows must stay narrow; "
                    f"broad scope phrase is reserved for {OBJECT_MODEL_FULL_REALIZATION_ID}: "
                    f"{phrase}"
                )

        evidence_paths = [
            str(item.get("path", ""))
            for item in row.get("evidence", [])
            if isinstance(item, dict)
        ]
        owner_paths = [
            str(path)
            for path in row.get("owner_modules", [])
            if isinstance(path, str)
        ]
        if not any(path.startswith(OBJECT_MODEL_RUNTIME_FIXTURE_PREFIX) for path in evidence_paths):
            raise CapabilityDocsError(
                f"{capability_id} object-model implemented rows require canonical "
                f"runtime object-model fixture evidence under "
                f"{OBJECT_MODEL_RUNTIME_FIXTURE_PREFIX}"
            )
        if not any(
            path.startswith(OBJECT_MODEL_RUNTIME_SOURCE_PREFIX)
            for path in [*evidence_paths, *owner_paths]
        ):
            raise CapabilityDocsError(
                f"{capability_id} object-model implemented rows require runtime "
                f"class/object-model source ownership under "
                f"{OBJECT_MODEL_RUNTIME_SOURCE_PREFIX}"
            )
