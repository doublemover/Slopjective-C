from __future__ import annotations

from dataclasses import dataclass

from .owners import (
    SOURCE_HYGIENE_PATTERN_OWNER,
    SOURCE_HYGIENE_PATTERN_OWNER_SURFACE,
)


@dataclass(frozen=True)
class ForbiddenPattern:
    pattern_id: str
    description: str
    regex: str
    severity: str = "error"
    include_paths: tuple[str, ...] = ()
    exclude_paths: tuple[str, ...] = ()
    residue_class: str = "general-hard-cutover-residue"
    gate_contract: str = "source-hygiene-hard-cutover"
    owner_id: str = SOURCE_HYGIENE_PATTERN_OWNER
    owner_surface: str = SOURCE_HYGIENE_PATTERN_OWNER_SURFACE
