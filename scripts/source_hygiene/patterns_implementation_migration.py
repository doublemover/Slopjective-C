from __future__ import annotations

from .pattern_model import ForbiddenPattern
from .pattern_scopes import IMPLEMENTATION_SOURCE_PATHS


IMPLEMENTATION_MIGRATION_PATTERNS: tuple[ForbiddenPattern, ...] = (
    ForbiddenPattern(
        "migration-implementation-surface",
        "Migration lanes, paths, modes, support adapters, and bridge surfaces are retired from active implementation code.",
        r"\bmigration[-_\s]+(?:lane|path|mode|support|bridge|adapter|wrapper|shim)s?\b"
        r"|\b(?:bridge|adapter|wrapper|shim|support)s?[-_\s]+migration\b",
        include_paths=IMPLEMENTATION_SOURCE_PATHS,
        residue_class="shim-fallback-language",
    ),
)
