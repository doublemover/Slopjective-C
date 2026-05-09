from __future__ import annotations

from .pattern_model import ForbiddenPattern


PUBLIC_ALIAS_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "backward-compatible-alias-wording",
        "Backward-compatible alias claims are removed from active hard-cutover surfaces.",
        r"\bbackward[-\s]+compatible\s+aliases\b",
        residue_class="alias-residue",
    ),
)
