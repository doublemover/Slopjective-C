from __future__ import annotations

from typing import Any, NamedTuple

from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.support_links import _row_support_claims


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
