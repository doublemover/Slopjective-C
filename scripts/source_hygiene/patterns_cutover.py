from __future__ import annotations

from .pattern_model import ForbiddenPattern
from .pattern_scopes import IMPLEMENTATION_SOURCE_PATHS


HARD_CUTOVER_RESIDUE_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "compatibility-wrapper-or-bridge-surface",
        "Compatibility wrappers, bridges, adapters, layers, facades, and shims are retired from active implementation code.",
        r"\bcompat(?:ibility)?[-_\s]+(?:wrapper|bridge|adapter|layer|facade|shim)s?\b"
        r"|\b(?:wrapper|bridge|adapter|layer|facade|shim)s?[-_\s]+compat(?:ibility)?\b",
        include_paths=IMPLEMENTATION_SOURCE_PATHS,
    ),
    ForbiddenPattern(
        "fallback-implementation-surface",
        "Fallback implementation paths are retired from active implementation code.",
        r"\bfallback[-_\s]+(?:implementation|handler|adapter|bridge|shim|wrapper|path|route|mode|layer|dispatch|hot[-_\s]+path)s?\b"
        r"|\b(?:implementation|handler|adapter|bridge|shim|wrapper|path|route|mode|layer|dispatch|hot[-_\s]+path)s?[-_\s]+fallback\b",
        include_paths=IMPLEMENTATION_SOURCE_PATHS,
    ),
    ForbiddenPattern(
        "migration-implementation-surface",
        "Migration lanes, paths, modes, support adapters, and bridge surfaces are retired from active implementation code.",
        r"\bmigration[-_\s]+(?:lane|path|mode|support|bridge|adapter|wrapper|shim)s?\b"
        r"|\b(?:bridge|adapter|wrapper|shim|support)s?[-_\s]+migration\b",
        include_paths=IMPLEMENTATION_SOURCE_PATHS,
    ),
    ForbiddenPattern(
        "legacy-compatibility-support-surface",
        "Legacy support and backward-compatibility surfaces are retired from active implementation code.",
        r"\blegacy[-_\s]+(?:support|mode|path|bridge|adapter|wrapper|shim)s?\b"
        r"|\b(?:backcompat|back[-_\s]?compat|backward[-_\s]+compat(?:ibility)?)\b",
        include_paths=IMPLEMENTATION_SOURCE_PATHS,
    ),
)
