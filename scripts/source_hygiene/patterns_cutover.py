from __future__ import annotations

from .patterns_implementation_fallbacks import IMPLEMENTATION_FALLBACK_PATTERNS
from .patterns_implementation_legacy import IMPLEMENTATION_LEGACY_PATTERNS
from .patterns_implementation_migration import IMPLEMENTATION_MIGRATION_PATTERNS
from .patterns_implementation_shims import IMPLEMENTATION_SHIM_PATTERNS


HARD_CUTOVER_RESIDUE_PATTERNS = (
    *IMPLEMENTATION_SHIM_PATTERNS,
    *IMPLEMENTATION_FALLBACK_PATTERNS,
    *IMPLEMENTATION_MIGRATION_PATTERNS,
    *IMPLEMENTATION_LEGACY_PATTERNS,
)
