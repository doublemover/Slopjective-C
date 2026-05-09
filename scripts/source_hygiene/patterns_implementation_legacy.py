from __future__ import annotations

from .pattern_model import ForbiddenPattern
from .pattern_scopes import IMPLEMENTATION_SOURCE_PATHS


IMPLEMENTATION_LEGACY_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "legacy-compatibility-support-surface",
        "Legacy support and backward-compatibility surfaces are retired from active implementation code.",
        r"\blegacy[-_\s]+(?:support|mode|path|bridge|adapter|wrapper|shim)s?\b"
        r"|\b(?:backcompat|back[-_\s]?compat|backward[-_\s]+compat(?:ibility)?)\b",
        include_paths=IMPLEMENTATION_SOURCE_PATHS,
        residue_class="legacy-compatibility-text",
    ),
)
