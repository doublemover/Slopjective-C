from __future__ import annotations

from .pattern_model import ForbiddenPattern
from .pattern_scopes import PUBLIC_SURFACE_PATHS


PUBLIC_SHIM_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "public-compatibility-shim-support-claim",
        "Compatibility-shim support claims are removed from public hard-cutover surfaces.",
        r"\bcompatibility[-_\s]+shim[^\n]{0,100}\b(?:support(?:ed|s|ing)?|accept(?:ed|s|ing)?|enable(?:d|s|ing)?|allow(?:ed|s|ing)?)\b"
        r"|\b(?:support(?:ed|s|ing)?|accept(?:ed|s|ing)?|enable(?:d|s|ing)?|allow(?:ed|s|ing)?)[^\n]{0,100}\bcompatibility[-_\s]+shim\b",
        include_paths=PUBLIC_SURFACE_PATHS,
        residue_class="shim-fallback-language",
    ),
    ForbiddenPattern(
        "shim-wording",
        "Shim wording is not allowed on authoritative active paths.",
        r"\bshim\b|\bcompatibility\s+shim\b",
        residue_class="shim-fallback-language",
    ),
)
