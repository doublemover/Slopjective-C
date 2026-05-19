from __future__ import annotations

from .pattern_model import ForbiddenPattern
from .pattern_scopes import PUBLIC_SURFACE_PATHS


PUBLIC_PROJECTION_PATTERNS: tuple[ForbiddenPattern, ...] = (
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
)
