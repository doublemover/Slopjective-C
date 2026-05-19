from __future__ import annotations

from scripts.source_hygiene.patterns_cutover import HARD_CUTOVER_RESIDUE_PATTERNS
from scripts.source_hygiene.patterns_implementation_fallbacks import (
    IMPLEMENTATION_FALLBACK_PATTERNS,
)
from scripts.source_hygiene.patterns_implementation_legacy import (
    IMPLEMENTATION_LEGACY_PATTERNS,
)
from scripts.source_hygiene.patterns_implementation_migration import (
    IMPLEMENTATION_MIGRATION_PATTERNS,
)
from scripts.source_hygiene.patterns_implementation_shims import (
    IMPLEMENTATION_SHIM_PATTERNS,
)
from scripts.source_hygiene.patterns_public_aliases import PUBLIC_ALIAS_PATTERNS
from scripts.source_hygiene.patterns_public_claims import PUBLIC_CLAIM_PATTERNS
from scripts.source_hygiene.patterns_public_fallbacks import PUBLIC_FALLBACK_PATTERNS
from scripts.source_hygiene.patterns_public_legacy import PUBLIC_LEGACY_PATTERNS
from scripts.source_hygiene.patterns_public_migration import PUBLIC_MIGRATION_PATTERNS
from scripts.source_hygiene.patterns_public_projection import (
    PUBLIC_PROJECTION_PATTERNS,
)
from scripts.source_hygiene.patterns_public_shims import PUBLIC_SHIM_PATTERNS


def assert_public_claim_patterns_are_split_by_owner_modules() -> None:
    owner_patterns = (
        *PUBLIC_ALIAS_PATTERNS,
        *PUBLIC_FALLBACK_PATTERNS,
        *PUBLIC_PROJECTION_PATTERNS,
        *PUBLIC_LEGACY_PATTERNS,
        *PUBLIC_SHIM_PATTERNS,
        *PUBLIC_MIGRATION_PATTERNS,
    )

    assert PUBLIC_CLAIM_PATTERNS == owner_patterns
    assert len({pattern.pattern_id for pattern in PUBLIC_CLAIM_PATTERNS}) == len(
        PUBLIC_CLAIM_PATTERNS
    )


def assert_implementation_residue_patterns_are_split_by_owner_modules() -> None:
    owner_patterns = (
        *IMPLEMENTATION_SHIM_PATTERNS,
        *IMPLEMENTATION_FALLBACK_PATTERNS,
        *IMPLEMENTATION_MIGRATION_PATTERNS,
        *IMPLEMENTATION_LEGACY_PATTERNS,
    )

    assert HARD_CUTOVER_RESIDUE_PATTERNS == owner_patterns
    assert len({pattern.pattern_id for pattern in HARD_CUTOVER_RESIDUE_PATTERNS}) == len(
        HARD_CUTOVER_RESIDUE_PATTERNS
    )
