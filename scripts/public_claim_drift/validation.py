"""Baseline and public-claim validation helpers."""

from __future__ import annotations

from typing import Any, Mapping, Sequence

from .constants import (
    CLAIM_KEYWORD_RE,
    FORBIDDEN_PATTERNS,
    GUARD_RE,
    PROMOTIONAL_CLAIM_RE,
    PUBLIC_CLAIM_DRIFT_BLOCKER_METADATA,
    PUBLIC_CLAIM_DRIFT_OWNER,
    PUBLIC_CLAIM_DRIFT_OWNER_SURFACE,
)
from .models import JsonObject


def public_claim_drift_owner_contract(
    *,
    public_surfaces: set[str],
    scan_paths: list[str],
) -> JsonObject:
    return {
        "owner_id": PUBLIC_CLAIM_DRIFT_OWNER,
        "owner_surface": PUBLIC_CLAIM_DRIFT_OWNER_SURFACE,
        "public_claim_surface_count": len(public_surfaces),
        "scan_path_count": len(scan_paths),
        "public_claim_surfaces": sorted(public_surfaces),
        "scan_paths": list(scan_paths),
        "blocker_metadata": dict(PUBLIC_CLAIM_DRIFT_BLOCKER_METADATA),
    }


def public_claim_scan_paths(support_summary: Mapping[str, Any]) -> list[str]:
    classifications = support_summary["classifications"]
    public_surfaces = set(support_summary["public_claim_surfaces"])
    return sorted(
        {
            *public_surfaces,
            *(
                surface
                for row in classifications
                if row.get("fail_closed")
                for surface in row.get("checked_in_surfaces", [])
            ),
        }
    )


def clipped(text: str, limit: int = 220) -> str:
    stripped = text.strip()
    if len(stripped) <= limit:
        return stripped
    return stripped[: limit - 3] + "..."


def context_window(lines: Sequence[str], index: int) -> str:
    context: list[str] = []
    cursor = index
    while cursor >= 0 and len(context) < 8:
        line = lines[cursor].strip()
        if line:
            context.append(line)
        cursor -= 1
    return " ".join(reversed(context))


def is_guarded(context: str) -> bool:
    return bool(GUARD_RE.search(context))


def scan_surface_lines(
    *,
    relative_path: str,
    lines: Sequence[str],
    rows_by_path: Mapping[str, list[Mapping[str, Any]]],
    envelope_row: Mapping[str, Any] | None,
    public_surfaces: set[str],
) -> tuple[list[JsonObject], list[JsonObject]]:
    claim_mappings: list[JsonObject] = []
    findings: list[JsonObject] = []
    for index, line in enumerate(lines):
        context = context_window(lines, index)
        guarded = is_guarded(context)
        for pattern_id, pattern in FORBIDDEN_PATTERNS:
            if pattern.search(line) and not guarded:
                findings.append(
                    {
                        "kind": "unguarded-forbidden-public-claim",
                        "path": relative_path,
                        "line": index + 1,
                        "pattern_id": pattern_id,
                        "claim_text": clipped(line),
                    }
                )

        if not CLAIM_KEYWORD_RE.search(line):
            continue

        matched_rows = list(rows_by_path.get(relative_path, []))
        if relative_path in public_surfaces and envelope_row and envelope_row not in matched_rows:
            matched_rows.append(envelope_row)
        evidence_families = sorted(
            {
                family
                for row in matched_rows
                if not row.get("fail_closed")
                for family in row.get("required_evidence_families", [])
            }
        )
        checked_surfaces = sorted(
            {
                surface
                for row in matched_rows
                if not row.get("fail_closed")
                for surface in row.get("checked_in_surfaces", [])
            }
        )
        fail_closed_surfaces = sorted(
            row.get("surface", "")
            for row in matched_rows
            if bool(row.get("fail_closed"))
        )
        promotional = bool(PROMOTIONAL_CLAIM_RE.search(line))
        if promotional and not guarded and not evidence_families:
            findings.append(
                {
                    "kind": "unmapped-promotional-public-claim",
                    "path": relative_path,
                    "line": index + 1,
                    "claim_text": clipped(line),
                }
            )
        claim_mappings.append(
            {
                "path": relative_path,
                "line": index + 1,
                "claim_text": clipped(line),
                "guarded": guarded,
                "promotional": promotional,
                "mapped_surfaces": [row.get("surface") for row in matched_rows],
                "evidence_families": evidence_families,
                "checked_in_surface_count": len(checked_surfaces),
                "fail_closed_surfaces": fail_closed_surfaces,
            }
        )
    return claim_mappings, findings


__all__ = [
    "clipped",
    "context_window",
    "is_guarded",
    "public_claim_drift_owner_contract",
    "public_claim_scan_paths",
    "scan_surface_lines",
]
