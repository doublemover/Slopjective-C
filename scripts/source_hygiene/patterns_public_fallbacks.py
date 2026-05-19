from __future__ import annotations

from .pattern_model import ForbiddenPattern
from .pattern_scopes import PUBLIC_SURFACE_PATHS


PUBLIC_FALLBACK_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "deterministic-fallback-wording",
        "Fallback wording is not allowed as active behavior documentation.",
        r"\bdeterministic\s+fallbacks?\b|\bfallback\s+paths?\b|\bfallback\s+dispatch(?:es)?\b|\bfallback\s+behaviors?\b",
        residue_class="shim-fallback-language",
    ),
    ForbiddenPattern(
        "fallback-only-wording",
        "Fallback-only wording is not allowed as active behavior documentation.",
        r"\bfallback[-\s]+only\b",
        residue_class="shim-fallback-language",
    ),
    ForbiddenPattern(
        "public-fallback-support-claim",
        "Fallback support claims are removed from public hard-cutover surfaces.",
        r"\bfallback(?:[-_\s]+(?:mode|path|behavior|route|dispatch))?[^\n]{0,100}\b(?:support(?:ed|s|ing)?|accept(?:ed|s|ing)?|enable(?:d|s|ing)?|allow(?:ed|s|ing)?)\b"
        r"|\b(?:support(?:ed|s|ing)?|accept(?:ed|s|ing)?|enable(?:d|s|ing)?|allow(?:ed|s|ing)?)[^\n]{0,100}\bfallback(?:[-_\s]+(?:mode|path|behavior|route|dispatch))?\b",
        include_paths=PUBLIC_SURFACE_PATHS,
        residue_class="shim-fallback-language",
    ),
)
