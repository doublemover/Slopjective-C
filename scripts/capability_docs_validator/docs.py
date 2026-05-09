from __future__ import annotations

from typing import Any

from capability_docs_validator.constants import EVIDENCE_DOC, MATRIX_DOC, SUPPORT_CLAIM_RE
from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.support_links import _row_support_claims


def _validate_docs_reference_rows(rows: list[dict[str, Any]]) -> None:
    matrix_text = MATRIX_DOC.read_text(encoding="utf-8")
    evidence_text = EVIDENCE_DOC.read_text(encoding="utf-8")
    docs_text = f"{matrix_text}\n{evidence_text}"
    matrix_support_claims: set[str] = set()
    for row in rows:
        capability_id = str(row["id"])
        if capability_id not in evidence_text:
            raise CapabilityDocsError(f"evidence map missing capability id: {capability_id}")
        for claim_id in _row_support_claims(row):
            matrix_support_claims.add(claim_id)
            if claim_id not in matrix_text:
                raise CapabilityDocsError(f"capability matrix doc missing support claim: {claim_id}")
            if claim_id not in evidence_text:
                raise CapabilityDocsError(f"evidence map missing support claim: {claim_id}")
        for item in row["evidence"]:
            path = str(item["path"])
            if path not in evidence_text and path not in matrix_text:
                raise CapabilityDocsError(f"docs missing evidence path for {capability_id}: {path}")
            command = item.get("command")
            if isinstance(command, str) and command and command not in docs_text:
                raise CapabilityDocsError(f"docs missing evidence command for {capability_id}: {command}")

    undocumented_claims = sorted(set(SUPPORT_CLAIM_RE.findall(docs_text)) - matrix_support_claims)
    if undocumented_claims:
        raise CapabilityDocsError(
            "support docs mention claims not declared in capability_matrix.json: "
            + ", ".join(undocumented_claims)
        )
