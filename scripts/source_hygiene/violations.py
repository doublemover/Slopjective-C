from __future__ import annotations

from typing import Any

from .pattern_model import ForbiddenPattern


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
    }


def format_violation(violation: dict[str, Any]) -> str:
    return (
        f"{violation['path']}:{violation['line']}: "
        f"{violation['pattern_id']}: {violation['excerpt']}"
    )
