from __future__ import annotations

from typing import Any

from .pattern_model import ForbiddenPattern

SOURCE_HYGIENE_VIOLATION_CONTRACT_ID = "source-hygiene-violation-record-v1"
SOURCE_HYGIENE_VIOLATION_OWNER_SURFACE = "scripts/source_hygiene/violations.py"
SOURCE_HYGIENE_VIOLATION_FIELDS: tuple[str, ...] = (
    "pattern_id",
    "severity",
    "description",
    "path",
    "line",
    "excerpt",
    "residue_class",
    "gate_contract",
    "pattern_owner",
    "pattern_owner_surface",
    "violation_contract",
    "violation_owner_surface",
)


def violation_contract_summary() -> dict[str, object]:
    return {
        "contract_id": SOURCE_HYGIENE_VIOLATION_CONTRACT_ID,
        "owner_surface": SOURCE_HYGIENE_VIOLATION_OWNER_SURFACE,
        "fields": list(SOURCE_HYGIENE_VIOLATION_FIELDS),
        "excerpt_limit": 240,
        "pattern_owner_required": True,
        "gate_contract_required": True,
    }


def build_pattern_violation(
    *,
    pattern: ForbiddenPattern,
    repo_path: str,
    line_number: int,
    line: str,
) -> dict[str, Any]:
    return {
        "pattern_id": pattern.pattern_id,
        "severity": pattern.severity,
        "description": pattern.description,
        "path": repo_path,
        "line": line_number,
        "excerpt": line.strip()[:240],
        "residue_class": pattern.residue_class,
        "gate_contract": pattern.gate_contract,
        "pattern_owner": pattern.owner_id,
        "pattern_owner_surface": pattern.owner_surface,
        "violation_contract": SOURCE_HYGIENE_VIOLATION_CONTRACT_ID,
        "violation_owner_surface": SOURCE_HYGIENE_VIOLATION_OWNER_SURFACE,
    }


def format_violation(violation: dict[str, Any]) -> str:
    return (
        f"{violation['path']}:{violation['line']}: "
        f"{violation['pattern_id']}: {violation['excerpt']}"
    )


__all__ = [
    "SOURCE_HYGIENE_VIOLATION_CONTRACT_ID",
    "SOURCE_HYGIENE_VIOLATION_FIELDS",
    "SOURCE_HYGIENE_VIOLATION_OWNER_SURFACE",
    "build_pattern_violation",
    "format_violation",
    "violation_contract_summary",
]
