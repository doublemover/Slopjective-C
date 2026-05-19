from __future__ import annotations

from .pattern_model import ForbiddenPattern
from .pattern_scopes import PUBLIC_SURFACE_PATHS


PUBLIC_LEGACY_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "legacy-spec-redirect-wording",
        "Legacy spec redirect claims are removed from active hard-cutover surfaces.",
        r"\blegacy\s+spec\s+redirects?\b",
        residue_class="legacy-compatibility-text",
    ),
    ForbiddenPattern(
        "legacy-compatibility-text",
        "Legacy-compatibility wording must not remain as active public support text.",
        r"\blegacy[-_\s]+compatibility(?:[-_\s]+(?:support|mode|path|layer|surface|text|contract|claim))?\b"
        r"|\bcompatibility[-_\s]+legacy(?:[-_\s]+(?:support|mode|path|layer|surface|text|contract|claim))?\b",
        include_paths=PUBLIC_SURFACE_PATHS,
        residue_class="legacy-compatibility-text",
    ),
    ForbiddenPattern(
        "old-mode-wording",
        "Old-mode wording is not allowed on authoritative active paths.",
        r"\bold\s+mode\b",
        residue_class="legacy-compatibility-text",
    ),
)
