from __future__ import annotations

from .pattern_model import ForbiddenPattern
from .pattern_scopes import PUBLIC_SURFACE_PATHS


PUBLIC_MIGRATION_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "public-migration-lane-support-claim",
        "Migration-lane support claims are removed from public hard-cutover surfaces.",
        r"\bmigration[-_\s]+lane[^\n]{0,120}\b(?:support(?:ed|s|ing)?|accept(?:ed|s|ing)?|enable(?:d|s|ing)?|allow(?:ed|s|ing)?|mode|path)\b"
        r"|\b(?:support(?:ed|s|ing)?|accept(?:ed|s|ing)?|enable(?:d|s|ing)?|allow(?:ed|s|ing)?|mode|path)[^\n]{0,120}\bmigration[-_\s]+lane\b",
        include_paths=PUBLIC_SURFACE_PATHS,
        residue_class="shim-fallback-language",
    ),
)
