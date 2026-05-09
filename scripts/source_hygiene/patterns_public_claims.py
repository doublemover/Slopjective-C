from __future__ import annotations

from .pattern_model import ForbiddenPattern
from .pattern_scopes import PUBLIC_SURFACE_PATHS


PUBLIC_CLAIM_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "backward-compatible-alias-wording",
        "Backward-compatible alias claims are removed from active hard-cutover surfaces.",
        r"\bbackward[-\s]+compatible\s+aliases\b",
        residue_class="alias-residue",
    ),
    ForbiddenPattern(
        "legacy-spec-redirect-wording",
        "Legacy spec redirect claims are removed from active hard-cutover surfaces.",
        r"\blegacy\s+spec\s+redirects?\b",
        residue_class="legacy-compatibility-text",
    ),
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
        "projected-retired-behavior-claim",
        "Projected support language must not turn retired hard-cutover residues into future behavior claims.",
        r"\b(?:future|planned|eventual(?:ly)?|would|will|should|target(?:ed)?)\b"
        r"[^\n]{0,120}\b(?:fallback|compatibility|shim|alias(?:es)?|migration[-_\s]+lane|old[-_\s]+mode|legacy[-_\s]+support|report[-_\s]+only)\b"
        r"|\b(?:fallback|compatibility|shim|alias(?:es)?|migration[-_\s]+lane|old[-_\s]+mode|legacy[-_\s]+support|report[-_\s]+only)\b"
        r"[^\n]{0,120}\b(?:future|planned|eventual(?:ly)?|would|will|should|target(?:ed)?)\b",
        include_paths=PUBLIC_SURFACE_PATHS,
        residue_class="projected-behavior-claim",
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
        "public-fallback-support-claim",
        "Fallback support claims are removed from public hard-cutover surfaces.",
        r"\bfallback(?:[-_\s]+(?:mode|path|behavior|route|dispatch))?[^\n]{0,100}\b(?:support(?:ed|s|ing)?|accept(?:ed|s|ing)?|enable(?:d|s|ing)?|allow(?:ed|s|ing)?)\b"
        r"|\b(?:support(?:ed|s|ing)?|accept(?:ed|s|ing)?|enable(?:d|s|ing)?|allow(?:ed|s|ing)?)[^\n]{0,100}\bfallback(?:[-_\s]+(?:mode|path|behavior|route|dispatch))?\b",
        include_paths=PUBLIC_SURFACE_PATHS,
        residue_class="shim-fallback-language",
    ),
    ForbiddenPattern(
        "shim-wording",
        "Shim wording is not allowed on authoritative active paths.",
        r"\bshim\b|\bcompatibility\s+shim\b",
        residue_class="shim-fallback-language",
    ),
    ForbiddenPattern(
        "public-compatibility-shim-support-claim",
        "Compatibility-shim support claims are removed from public hard-cutover surfaces.",
        r"\bcompatibility[-_\s]+shim[^\n]{0,100}\b(?:support(?:ed|s|ing)?|accept(?:ed|s|ing)?|enable(?:d|s|ing)?|allow(?:ed|s|ing)?)\b"
        r"|\b(?:support(?:ed|s|ing)?|accept(?:ed|s|ing)?|enable(?:d|s|ing)?|allow(?:ed|s|ing)?)[^\n]{0,100}\bcompatibility[-_\s]+shim\b",
        include_paths=PUBLIC_SURFACE_PATHS,
        residue_class="shim-fallback-language",
    ),
    ForbiddenPattern(
        "public-migration-lane-support-claim",
        "Migration-lane support claims are removed from public hard-cutover surfaces.",
        r"\bmigration[-_\s]+lane[^\n]{0,120}\b(?:support(?:ed|s|ing)?|accept(?:ed|s|ing)?|enable(?:d|s|ing)?|allow(?:ed|s|ing)?|mode|path)\b"
        r"|\b(?:support(?:ed|s|ing)?|accept(?:ed|s|ing)?|enable(?:d|s|ing)?|allow(?:ed|s|ing)?|mode|path)[^\n]{0,120}\bmigration[-_\s]+lane\b",
        include_paths=PUBLIC_SURFACE_PATHS,
        residue_class="shim-fallback-language",
    ),
    ForbiddenPattern(
        "old-mode-wording",
        "Old-mode wording is not allowed on authoritative active paths.",
        r"\bold\s+mode\b",
        residue_class="legacy-compatibility-text",
    ),
)
