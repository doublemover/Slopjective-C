from __future__ import annotations

from .patterns_public_aliases import PUBLIC_ALIAS_PATTERNS
from .patterns_public_fallbacks import PUBLIC_FALLBACK_PATTERNS
from .patterns_public_legacy import PUBLIC_LEGACY_PATTERNS
from .patterns_public_migration import PUBLIC_MIGRATION_PATTERNS
from .patterns_public_projection import PUBLIC_PROJECTION_PATTERNS
from .patterns_public_shims import PUBLIC_SHIM_PATTERNS


PUBLIC_CLAIM_PATTERNS = (
    *PUBLIC_ALIAS_PATTERNS,
    *PUBLIC_FALLBACK_PATTERNS,
    *PUBLIC_PROJECTION_PATTERNS,
    *PUBLIC_LEGACY_PATTERNS,
    *PUBLIC_SHIM_PATTERNS,
    *PUBLIC_MIGRATION_PATTERNS,
)
