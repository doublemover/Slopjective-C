from __future__ import annotations

from .pattern_model import ForbiddenPattern
from .pattern_scopes import IMPLEMENTATION_SOURCE_PATHS


IMPLEMENTATION_SHIM_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "compatibility-wrapper-or-bridge-surface",
        "Compatibility wrappers, bridges, adapters, layers, facades, and shims are retired from active implementation code.",
        r"\bcompat(?:ibility)?[-_\s]+(?:wrapper|bridge|adapter|layer|facade|shim)s?\b"
        r"|\b(?:wrapper|bridge|adapter|layer|facade|shim)s?[-_\s]+compat(?:ibility)?\b",
        include_paths=IMPLEMENTATION_SOURCE_PATHS,
        residue_class="shim-fallback-language",
    ),
)
