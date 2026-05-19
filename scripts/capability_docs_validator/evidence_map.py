from __future__ import annotations

from typing import Any, NamedTuple

from objc3c_tooling.paths import repo_rel

from capability_docs_validator.constants import (
    CAPABILITY_EVIDENCE_MAP_SCHEMA_ID,
    MATRIX_PATH,
    SCHEMA_PATH,
)
from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.support_links import _row_support_claims


EVIDENCE_MAP_ROW_KEY = (
    "capability_id",
    "support_claim",
    "evidence_kind",
    "path",
    "command",
)


def _projection_contract() -> dict[str, Any]:
    return {
        "source": "docs/support/capability_matrix.json#/capabilities/*/evidence",
        "owner": "scripts/capability_docs_validator/evidence_map.py",
        "row_key": list(EVIDENCE_MAP_ROW_KEY),
        "drift_rule": (
            "The evidence map is a flattened projection of capability matrix evidence rows. "
            "Validators fail on duplicate, missing, or extra row keys."
        ),
    }


def _evidence_policy() -> dict[str, Any]:
    return {
        "public_command_surface": "npm run objc3c -- <action>",
        "command_required_for": [
            "replayable implemented behavior evidence",
        ],
        "command_forbidden_for": [
            "source ownership rows",
            "schema ownership rows",
            "doc boundary rows",
            "diagnostic inventory rows that are not public replay commands",
        ],
        "row_role_rule": (
            "Rows without command are ownership or boundary evidence; they do not define public "
            "workflow surface or broaden capability state. Hard-cutover issue evidence and "
            "payload rows identify checked-in branch evidence boundaries only; their "
            "implementation commit lists do not prove validation, push state, GitHub issue "
            "edits, remote closure, or compatibility support."
        ),
    }


class EvidenceRowKey(NamedTuple):
    capability_id: str
    support_claim: str
    evidence_kind: str
    path: str
    command: str


def _key_text(key: EvidenceRowKey) -> str:
    support_claim = key.support_claim or "<none>"
    command = key.command or "<none>"
    return (
        f"{key.capability_id} | {support_claim} | "
        f"{key.evidence_kind} | {key.path} | {command}"
    )


def _matrix_evidence_row_keys(rows: list[dict[str, Any]]) -> list[EvidenceRowKey]:
    keys: list[EvidenceRowKey] = []
    for row in rows:
        capability_id = str(row["id"])
        support_claims = _row_support_claims(row)
        if not support_claims:
            support_claims = [""]
        for evidence in row["evidence"]:
            if not isinstance(evidence, dict):
                raise CapabilityDocsError(f"{capability_id} evidence entries must be objects")
            kind = evidence.get("kind")
            path = evidence.get("path")
            command = evidence.get("command", "")
            if not isinstance(kind, str) or not kind:
                raise CapabilityDocsError(f"{capability_id} evidence kind must be non-empty")
            if not isinstance(path, str) or not path:
                raise CapabilityDocsError(f"{capability_id} evidence path must be non-empty")
            if command != "" and not isinstance(command, str):
                raise CapabilityDocsError(f"{capability_id} evidence command must be a string")
            for support_claim in support_claims:
                keys.append(
                    EvidenceRowKey(
                        capability_id=capability_id,
                        support_claim=support_claim,
                        evidence_kind=kind,
                        path=path,
                        command=command,
                    )
                )
    return keys


def _evidence_map_row_keys(evidence_map: dict[str, Any]) -> list[EvidenceRowKey]:
    raw_rows = evidence_map.get("rows")
    if not isinstance(raw_rows, list):
        raise CapabilityDocsError("evidence map rows must be a list")
    keys: list[EvidenceRowKey] = []
    for index, row in enumerate(raw_rows):
        if not isinstance(row, dict):
            raise CapabilityDocsError(f"evidence map rows[{index}] must be an object")
        capability_id = row.get("capability_id")
        kind = row.get("evidence_kind")
        path = row.get("path")
        support_claim = row.get("support_claim", "")
        command = row.get("command", "")
        if not isinstance(capability_id, str) or not capability_id:
            raise CapabilityDocsError(f"evidence map rows[{index}].capability_id must be non-empty")
        if support_claim != "" and not isinstance(support_claim, str):
            raise CapabilityDocsError(f"{capability_id} support_claim must be a string")
        if not isinstance(kind, str) or not kind:
            raise CapabilityDocsError(f"{capability_id} evidence_kind must be non-empty")
        if not isinstance(path, str) or not path:
            raise CapabilityDocsError(f"{capability_id} path must be non-empty")
        if command != "" and not isinstance(command, str):
            raise CapabilityDocsError(f"{capability_id} command must be a string")
        keys.append(
            EvidenceRowKey(
                capability_id=capability_id,
                support_claim=support_claim,
                evidence_kind=kind,
                path=path,
                command=command,
            )
        )
    return keys


def _duplicates(keys: list[EvidenceRowKey]) -> list[EvidenceRowKey]:
    seen: set[EvidenceRowKey] = set()
    duplicates: list[EvidenceRowKey] = []
    for key in keys:
        if key in seen:
            duplicates.append(key)
        else:
            seen.add(key)
    return duplicates


def _row_from_key(key: EvidenceRowKey) -> dict[str, str]:
    row = {
        "capability_id": key.capability_id,
    }
    if key.support_claim:
        row["support_claim"] = key.support_claim
    row.update(
        {
            "evidence_kind": key.evidence_kind,
            "path": key.path,
        }
    )
    if key.command:
        row["command"] = key.command
    return row


def build_evidence_map_projection(matrix_rows: list[dict[str, Any]]) -> dict[str, Any]:
    return {
        "schema_version": CAPABILITY_EVIDENCE_MAP_SCHEMA_ID,
        "matrix_path": repo_rel(MATRIX_PATH),
        "matrix_schema_path": repo_rel(SCHEMA_PATH),
        "projection_contract": _projection_contract(),
        "evidence_policy": _evidence_policy(),
        "rows": [_row_from_key(key) for key in _matrix_evidence_row_keys(matrix_rows)],
    }


def _projection_drift_fields(expected: dict[str, Any], actual: dict[str, Any]) -> list[str]:
    fields = (
        "schema_version",
        "matrix_path",
        "matrix_schema_path",
        "projection_contract",
        "evidence_policy",
        "rows",
    )
    return [
        field
        for field in fields
        if actual.get(field) != expected.get(field)
    ]


def _validate_evidence_map_projection(
    matrix_rows: list[dict[str, Any]], evidence_map: dict[str, Any]
) -> None:
    matrix_keys = _matrix_evidence_row_keys(matrix_rows)
    evidence_keys = _evidence_map_row_keys(evidence_map)

    duplicate_matrix_keys = _duplicates(matrix_keys)
    if duplicate_matrix_keys:
        raise CapabilityDocsError(
            "capability matrix has duplicate evidence rows: "
            + "; ".join(_key_text(key) for key in duplicate_matrix_keys)
        )

    duplicate_evidence_keys = _duplicates(evidence_keys)
    if duplicate_evidence_keys:
        raise CapabilityDocsError(
            "evidence map has duplicate rows: "
            + "; ".join(_key_text(key) for key in duplicate_evidence_keys)
        )

    missing = sorted(set(matrix_keys) - set(evidence_keys), key=_key_text)
    extra = sorted(set(evidence_keys) - set(matrix_keys), key=_key_text)
    if missing or extra:
        details: list[str] = []
        if missing:
            details.append("missing evidence-map rows: " + "; ".join(_key_text(key) for key in missing))
        if extra:
            details.append("evidence-map rows not present in matrix: " + "; ".join(_key_text(key) for key in extra))
        raise CapabilityDocsError("evidence map drifted from capability matrix: " + " | ".join(details))

    expected = build_evidence_map_projection(matrix_rows)
    drift_fields = _projection_drift_fields(expected, evidence_map)
    if drift_fields:
        raise CapabilityDocsError(
            "evidence map drifted from canonical projection owned by "
            "scripts/capability_docs_validator/evidence_map.py: "
            + ", ".join(drift_fields)
        )
