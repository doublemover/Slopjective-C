from __future__ import annotations

from .pattern_model import ForbiddenPattern
from .pattern_scopes import IMPLEMENTATION_SOURCE_PATHS


IMPLEMENTATION_FALLBACK_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "fallback-implementation-surface",
        "Fallback implementation paths are retired from active implementation code.",
        r"\bfallback[-_\s]+(?:implementation|handler|adapter|bridge|shim|wrapper|path|route|mode|layer|dispatch|hot[-_\s]+path)s?\b"
        r"|\b(?:implementation|handler|adapter|bridge|shim|wrapper|path|route|mode|layer|dispatch|hot[-_\s]+path)s?[-_\s]+fallback\b",
        include_paths=IMPLEMENTATION_SOURCE_PATHS,
        residue_class="shim-fallback-language",
    ),
)
